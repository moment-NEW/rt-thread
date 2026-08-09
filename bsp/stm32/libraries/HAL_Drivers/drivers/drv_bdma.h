/*
 * Copyright (c) 2006-2026, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-07-22     moment-NEW   first version
 */
#ifndef __DRV_BDMA_H_
#define __DRV_BDMA_H_

#include <rtthread.h>
#include <board.h>

#if defined(BSP_USING_BDMA) && defined(HAL_DMA_MODULE_ENABLED)
/*
 * NOTE: The only MCU series available for BDMA testing is STM32H7,
 * so only one series is supported now. More series can be added here
 * if needed.
 */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#ifndef STM32_BDMA_DEFAULT_PREEMPT_PRIORITY
#define STM32_BDMA_DEFAULT_PREEMPT_PRIORITY  5
#endif  /* STM32_BDMA_DEFAULT_PREEMPT_PRIORITY */

#ifndef STM32_BDMA_DEFAULT_PRIORITY
#define STM32_BDMA_DEFAULT_PRIORITY          DMA_PRIORITY_LOW
#endif  /* STM32_BDMA_DEFAULT_PRIORITY */

#ifndef STM32_BDMA_DEFAULT_SUB_PRIORITY
#define STM32_BDMA_DEFAULT_SUB_PRIORITY      0
#endif  /* STM32_BDMA_DEFAULT_SUB_PRIORITY */

#if defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32H7RS)
/**
 * @brief Static BDMA endpoint description used by board-level config headers.
 */
struct stm32_bdma_config
{
    void *Instance;                      /**< BDMA channel instance pointer. */
    rt_uint32_t dma_rcc;                 /**< RCC enable bit for the BDMA controller. */
    IRQn_Type dma_irq;                   /**< NVIC IRQ number for the BDMA channel. */
    rt_uint32_t priority;                /**< BDMA transfer priority. */
    rt_uint8_t preempt_priority;         /**< NVIC preempt priority for the BDMA IRQ. */
    rt_uint8_t sub_priority;             /**< NVIC sub priority for the BDMA IRQ. */
    rt_uint32_t request;                 /**< BDMA request selector. */
    rt_uint32_t direction;               /**< BDMA transfer direction. */
    rt_uint32_t periph_inc;              /**< Peripheral address increment mode. */
    rt_uint32_t mem_inc;                 /**< Memory address increment mode. */
    rt_uint32_t periph_data_alignment;   /**< Peripheral data alignment. */
    rt_uint32_t mem_data_alignment;      /**< Memory data alignment. */
    rt_uint32_t mode;                    /**< BDMA transfer mode. */
};


rt_err_t stm32_bdma_init(DMA_HandleTypeDef *handle,
                         const struct stm32_bdma_config *config);
rt_err_t stm32_bdma_setup(DMA_HandleTypeDef *handle,
                          void *parent_handle,
                          DMA_HandleTypeDef **dma_slot,
                          const struct stm32_bdma_config *config);

rt_err_t stm32_bdma_deinit(DMA_HandleTypeDef *handle,
                           const struct stm32_bdma_config *config,
                           rt_bool_t abort_first);
#endif /* defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32H7RS) */



/* BDMA channels encode the channel number in the Instance pointer
 * (BDMA_Channel0 ... BDMA_Channel7), so there is no separate channel
 * selector field in the HAL DMA descriptor and the initializer takes
 * no channel argument.
 */
#define STM32_BDMA_CONFIG_INIT_EX(_instance, _dma_rcc, _dma_irq, _request, _priority, _preempt_priority, _sub_priority, _direction, _periph_inc, _mem_inc, _periph_data_alignment, _mem_data_alignment, _mode) \
    {                                                                                                                                                                                                  \
        .Instance = (_instance),                                                                                                                                                                       \
        .dma_rcc = (_dma_rcc),                                                                                                                                                                         \
        .dma_irq = (_dma_irq),                                                                                                                                                                         \
        .priority = (_priority),                                                                                                                                                                       \
        .preempt_priority = (_preempt_priority),                                                                                                                                                       \
        .sub_priority = (_sub_priority),                                                                                                                                                               \
        .request = (_request),                                                                                                                                                                         \
        .direction = (_direction),                                                                                                                                                                     \
        .periph_inc = (_periph_inc),                                                                                                                                                                   \
        .mem_inc = (_mem_inc),                                                                                                                                                                         \
        .periph_data_alignment = (_periph_data_alignment),                                                                                                                                             \
        .mem_data_alignment = (_mem_data_alignment),                                                                                                                                                   \
        .mode = (_mode)                                                                                                                                                                                \
    }

#define STM32_BDMA_RX_BYTE_CONFIG_INIT_EX(_instance, _dma_rcc, _dma_irq, _request, _priority, _preempt_priority, _sub_priority) \
    STM32_BDMA_CONFIG_INIT_EX((_instance), (_dma_rcc), (_dma_irq), (_request), (_priority), (_preempt_priority), (_sub_priority), DMA_PERIPH_TO_MEMORY, DMA_PINC_DISABLE, DMA_MINC_ENABLE, DMA_PDATAALIGN_BYTE, DMA_MDATAALIGN_BYTE, DMA_NORMAL)

#define STM32_BDMA_TX_BYTE_CONFIG_INIT_EX(_instance, _dma_rcc, _dma_irq, _request, _priority, _preempt_priority, _sub_priority) \
    STM32_BDMA_CONFIG_INIT_EX((_instance), (_dma_rcc), (_dma_irq), (_request), (_priority), (_preempt_priority), (_sub_priority), DMA_MEMORY_TO_PERIPH, DMA_PINC_DISABLE, DMA_MINC_ENABLE, DMA_PDATAALIGN_BYTE, DMA_MDATAALIGN_BYTE, DMA_NORMAL)





















#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* defined(BSP_USING_BDMA) && defined(HAL_DMA_MODULE_ENABLED) */
#endif /* __DRV_BDMA_H_ */

