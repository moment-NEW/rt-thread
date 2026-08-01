#define LOG_TAG              "drv.bdma"
#include <drv_log.h>

#include "drv_bdma.h"






//由于BDMA和DMA的配置有很多相似之处，所以后续可以考虑增加一些公有API
//一个小问题是BDMA使用与DMA相同的HAL库，因此必然触发HAL_DMA_MODULE_ENABLED
//这个宏会导致DMA相关的代码被编译进来，在某些情况下可能是不必要的，并且如果要添加DMA的配置宏，大量userspcace会被破坏
//可能的解决方案是使用更复杂的宏？或者修改Kconfig/Scons配置方式，或者重构BDMA使其直接使用LL库之类
//暂且搁置。
//暂记：可能需要和DMA配置联动，如果DMA已经配置了部分，就不用重新配置之类

static void stm32_bdma_enable_clock(rt_uint32_t dma_rcc)
{
    rt_uint32_t tmpreg;
    if (dma_rcc == 0)
    {
        LOG_E("bdma enable clock failed, dma_rcc is 0");
        __HAL_RCC_BDMA_CLK_ENABLE();//开启默认BDMA，避免空注册
        return;
    }
    SET_BIT(RCC->AHB4ENR, dma_rcc);
    tmpreg = READ_BIT(RCC->AHB4ENR, dma_rcc);
    UNUSED(tmpreg);
}

static void stm32_bdma_apply_config(DMA_HandleTypeDef *bdma_handle,
                              const struct stm32_bdma_config *bdma_config)
{
    RT_ASSERT(bdma_handle != RT_NULL);
    RT_ASSERT(bdma_config != RT_NULL);

    bdma_handle->Instance = bdma_config->Instance;
    bdma_handle->Init.Request = bdma_config->Request;
    bdma_handle->Init.Direction = bdma_config->Direction;
    bdma_handle->Init.PeriphInc = bdma_config->PeriphInc;
    bdma_handle->Init.MemInc = bdma_config->MemInc;
    bdma_handle->Init.PeriphDataAlignment = bdma_config->PeriphDataAlignment;
    bdma_handle->Init.MemDataAlignment = bdma_config->MemDataAlignment;
    bdma_handle->Init.Mode = bdma_config->Mode;
    bdma_handle->Init.Priority = bdma_config->Priority;
    //bdma has no fifo or burst support, so no need to configure those fields

    
}
/**
 * @brief Release one DMA IRQ line and disable it when no user remains.
 * @param dma_irq DMA IRQ number to release.
 */
static void stm32_dma_irq_put(IRQn_Type dma_irq)
{
#if defined(STM32_DMA_HAS_SHARED_IRQ_REFCNT)
    rt_base_t level;

    if (stm32_dma_irq_needs_refcount(dma_irq) && stm32_dma_irq_is_valid(dma_irq))
    {
        level = rt_hw_interrupt_disable();
        if (stm32_dma_irq_ref_count[(rt_uint32_t)dma_irq] > 0U)
        {
            stm32_dma_irq_ref_count[(rt_uint32_t)dma_irq]--;
            if (stm32_dma_irq_ref_count[(rt_uint32_t)dma_irq] == 0U)
            {
                HAL_NVIC_DisableIRQ(dma_irq);
            }
        }
        rt_hw_interrupt_enable(level);
        return;
    }
#endif /* defined(STM32_DMA_HAS_SHARED_IRQ_REFCNT) */

    HAL_NVIC_DisableIRQ(dma_irq);
}

/**
 * @brief Enable one DMA controller, apply the static descriptor and initialize HAL state.
 * @param dma_handle DMA handle owned by one peripheral driver.
 * @param dma_config Board-level DMA endpoint description.
 * @retval RT_EOK Initialization succeeded.
 * @retval -RT_ERROR HAL initialization failed.
 */
rt_err_t stm32_bdma_init(DMA_HandleTypeDef *bdma_handle,
                         const struct stm32_bdma_config *bdma_config)
{
    RT_ASSERT(bdma_handle != RT_NULL);
    RT_ASSERT(bdma_config != RT_NULL);

    stm32_bdma_enable_clock(bdma_config->dma_rcc);
    stm32_bdma_apply_config(bdma_handle, bdma_config);

    LOG_D("bdma init, bdma=%p, irq=%d", bdma_handle->Instance, bdma_config->dma_irq);

    if (HAL_DMA_DeInit(bdma_handle) != HAL_OK)
    {
        LOG_E("bdma deinit failed, bdma=%p, irq=%d", bdma_handle->Instance, bdma_config->dma_irq);
        return -RT_ERROR;
    }

    if (HAL_DMA_Init(bdma_handle) != HAL_OK)
    {
        LOG_E("bdma init failed, bdma=%p, irq=%d", bdma_handle->Instance, bdma_config->dma_irq);
        return -RT_ERROR;
    }

    return RT_EOK;
}


rt_err_t stm32_bdma_setup(DMA_HandleTypeDef *bdma_handle,
                          void *parent_handle,
                          DMA_HandleTypeDef **dma_slot,
                          const struct stm32_bdma_config *bdma_config)
{
    RT_ASSERT(bdma_handle != RT_NULL);
    RT_ASSERT(parent_handle != RT_NULL);
    RT_ASSERT(dma_slot != RT_NULL);
    RT_ASSERT(bdma_config != RT_NULL);

    rt_base_t level;
    if (stm32_bdma_init(bdma_handle, bdma_config) != RT_EOK)
    {
        LOG_E("bdma setup failed, bdma=%p, irq=%d", bdma_handle->Instance, bdma_config->dma_irq);
        return -RT_ERROR;
    }

    *dma_slot = bdma_handle;
    bdma_handle->Parent = parent_handle;
    
    level = rt_hw_interrupt_disable();//避免释放/申请DMA的同时错误读写IRQ

    HAL_NVIC_SetPriority(bdma_config->dma_irq, 
        bdma_config->preempt_priority, 0);//for convenient,subpriority is set to 0
       
    HAL_NVIC_EnableIRQ(bdma_config->dma_irq);

    rt_hw_interrupt_enable(level);
    return RT_EOK;
}


rt_err_t stm32_bdma_deinit(DMA_HandleTypeDef *bdma_handle,
                           const struct stm32_bdma_config *bdma_config,
                           rt_bool_t abort_first)
{
    RT_ASSERT(bdma_handle != RT_NULL);
    RT_ASSERT(bdma_config != RT_NULL);

    if (abort_first)
    {
        if (HAL_DMA_Abort(bdma_handle) != HAL_OK)
        {
            LOG_E("bdma abort failed, bdma=%p, irq=%d", bdma_handle->Instance, bdma_config->dma_irq);
            return -RT_ERROR;
        }
    }

    if (HAL_DMA_DeInit(bdma_handle) != HAL_OK)
    {
        LOG_E("bdma deinit failed, bdma=%p, irq=%d", bdma_handle->Instance, bdma_config->dma_irq);
        return -RT_ERROR;
    }

    stm32_dma_irq_put(bdma_config->dma_irq);

    LOG_D("bdma deinit, bdma=%p, irq=%d", bdma_handle->Instance, bdma_config->dma_irq);

    return RT_EOK;
}