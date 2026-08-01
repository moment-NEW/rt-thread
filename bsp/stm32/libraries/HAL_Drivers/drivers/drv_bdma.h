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

#if defined(RT_USING_BDMA) && defined(HAL_DMA_MODULE_ENABLED)
/*NOTE:the only mcu series that I can test BDMA on is STM32H7 */
/*so there's only one series supported now, if needed, you can add more series here*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#ifndef STM32_BDMA_DEFAULT_PREEMPT_PRIORITY
#define STM32_BDMA_DEFAULT_PREEMPT_PRIORITY  5
#endif  /* STM32_BDMA_DEFAULT_PREEMPT_PRIORITY */
#if defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32H7RS)

rt_err_t stm32_bdma_init(DMA_HandleTypeDef *handle,
                         const struct stm32_bdma_config *config);
rt_err_t stm32_bdma_setup(DMA_HandleTypeDef *handle,
                          void *parent_handle,
                          DMA_HandleTypeDef **dma_slot,
                          const struct stm32_bdma_config *config);
                    
rt_err_t stm32_bdma_deinit(DMA_HandleTypeDef *handle,
                           const struct stm32_bdma_config *config,
                           rt_bool_t abort_first);
#define STM32_BDMA_USES_REQUEST
#endif /* defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32H7RS) */









/*structs*/

struct stm32_bdma_config
{
    DMA_Channel_TypeDef *Instance;       /**< BDMA controller instance pointer. */
    rt_uint32_t dma_rcc;                /**< RCC enable bit for the BDMA controller. */
    IRQn_Type dma_irq;                /**< NVIC IRQ number for the BDMA controller. */
    rt_uint8_t preempt_priority;
    rt_uint32_t Request;        
    rt_uint32_t Direction;            
    rt_uint32_t PeriphInc;            
    rt_uint32_t MemInc;               
    rt_uint32_t PeriphDataAlignment;  
    rt_uint32_t MemDataAlignment;     
    rt_uint32_t Mode;                 
    rt_uint32_t Priority;             
              
   
};














#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* defined(RT_USING_BDMA) && defined(HAL_DMA_MODULE_ENABLED) */
#endif /* __DRV_BDMA_H_ */
