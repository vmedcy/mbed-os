/*
 * $ Copyright Cypress Semiconductor Apache2 $
 */

/** @file
 *  Implementation of whd_rtos_cmsis.c for CMSIS
 *
 */
#include "whd_rtos.h"
#include "whd_rtos_interface.h"

/******************************************************
*             Function definitions
******************************************************/

whd_result_t whd_rtos_create_thread_with_arg(whd_thread_type_t *thread, void (*entry_function)(
                                                 uint32_t), const char *name, void *stack, uint32_t stack_size, uint32_t priority,
                                             uint32_t arg)
{
    osThreadAttr_t atr;

    atr.name = name;
    atr.attr_bits = osThreadDetached;
    atr.priority = (osPriority_t)priority;
    atr.stack_mem = stack;
    atr.stack_size = stack_size;
    atr.cb_mem = NULL;
    atr.cb_size = 0;
    atr.tz_module = 0;
    atr.reserved = 0;

    *thread = osThreadNew( (osThreadFunc_t)entry_function, (void *)arg, &atr );

    if (*thread == NULL)
    {
        WPRINT_WHD_ERROR( (" thread init failed \n") );
    }
    return (*thread == NULL) ? WHD_THREAD_CREATE_FAILED : WHD_SUCCESS;

}

/**
 * Terminates the current thread
 *
 *
 * @param thread         : handle of the thread to terminate
 *
 * @returns WHD_SUCCESS on success, error otherwise
 */
whd_result_t whd_rtos_finish_thread(whd_thread_type_t *thread)
{
    osStatus_t status;

    if (*thread == NULL)
        return WHD_BADARG;

    status = osThreadTerminate(*thread);                         // stop the thread
    *thread = NULL;

    if (status == osOK)
        return WHD_SUCCESS;
    else
        return WHD_THREAD_FINISH_FAIL;
}

/**
 * Deletes a terminated thread
 *
 *
 * @param thread         : handle of the terminated thread to delete
 *
 * @returns WHD_SUCCESS on success, error otherwise
 */
whd_result_t whd_rtos_delete_terminated_thread(whd_thread_type_t *thread)
{
    osStatus_t status;
    if (*thread == NULL)
        return WHD_BADARG;

    status = osThreadTerminate(*thread);                         // stop the thread
    *thread = NULL;

    if (status == osOK)
        return WHD_SUCCESS;
    else
        return WHD_THREAD_DELETE_FAIL;
}

/**
 * Blocks the current thread until the indicated thread is complete
 *
 * @param thread         : handle of the thread to terminate
 *
 * @returns WHD_SUCCESS on success, error otherwise
 */
whd_result_t whd_rtos_join_thread(whd_thread_type_t *thread)
{
    osStatus_t status;
    if (*thread == NULL)
        return WHD_BADARG;

    status = osThreadJoin(*thread);

    if (status == osOK)
        return WHD_SUCCESS;
    else
        return WHD_INVALID_JOIN_STATUS;
}

/**
 * Creates a semaphore
 *
 * @param semaphore         : pointer to variable which will receive handle of created semaphore
 *
 * @returns WHD_SUCCESS on success, error otherwise
 */
whd_result_t whd_rtos_init_semaphore(whd_semaphore_type_t *semaphore)
{
    *semaphore = osSemaphoreNew(1, 0, NULL);
    if (*semaphore == NULL)
    {
        WPRINT_WHD_ERROR( (" semaphore init failed \n") );
        return WHD_SEMAPHORE_ERROR;
    }
    return WHD_SUCCESS;
}

/**
 * Gets a semaphore
 *
 * If value of semaphore is larger than zero, then the semaphore is decremented and function returns
 * Else If value of semaphore is zero, then current thread is suspended until semaphore is set.
 * Value of semaphore should never be below zero
 *
 * Must not be called from interrupt context, since it could block, and since an interrupt is not a
 * normal thread, so could cause RTOS problems if it tries to suspend it.
 * If called from interrupt context time out value should be 0.
 *
 * @param semaphore   : Pointer to variable which will receive handle of created semaphore
 * @param timeout_ms  : Maximum period to block for. Can be passed NEVER_TIMEOUT to request no timeout
 * @param will_set_in_isr : True if the semaphore will be set in an ISR. Currently only used for NoOS/NoNS
 *
 */
whd_result_t whd_rtos_get_semaphore(whd_semaphore_type_t *semaphore, uint32_t timeout_ms,
                                    whd_bool_t will_set_in_isr)
{
    osStatus_t result;
    UNUSED_PARAMETER(will_set_in_isr);

    if (*semaphore == NULL)
    {
        WPRINT_WHD_ERROR( (" null semaphore id \n") );
        return WHD_SEMAPHORE_ERROR;
    }

    if (timeout_ms == NEVER_TIMEOUT)
        result = osSemaphoreAcquire(*semaphore, osWaitForever);
    else
        result = osSemaphoreAcquire(*semaphore, timeout_ms * SYSTICK_FREQUENCY / 1000);

    if (result == osOK)
    {
        return WHD_SUCCESS;
    }
    else if (result == osErrorTimeout)
    {
        return WHD_TIMEOUT;
    }
    else if (result == osErrorResource)
    {
        WPRINT_WHD_ERROR( ("%s semaphore resource error\n", __func__) );
        return WHD_WAIT_ABORTED;
    }
    return WHD_SEMAPHORE_ERROR;
}

/**
 * Sets a semaphore
 *
 * If any threads are waiting on the semaphore, the first thread is resumed
 * Else increment semaphore.
 *
 * Can be called from interrupt context, so must be able to handle resuming other
 * threads from interrupt context.
 *
 * @param semaphore       : Pointer to variable which will receive handle of created semaphore
 * @param called_from_ISR : Value of WHD_TRUE indicates calling from interrupt context
 *                          Value of WHD_FALSE indicates calling from normal thread context
 *
 * @return whd_result_t : WHD_SUCCESS if semaphore was successfully set
 *                        : error if an error occurred
 *
 */
whd_result_t whd_rtos_set_semaphore(whd_semaphore_type_t *semaphore, whd_bool_t called_from_ISR)
{
    osStatus_t result;
    UNUSED_PARAMETER(called_from_ISR);

    if (*semaphore == NULL)
    {
        WPRINT_WHD_ERROR( (" null semaphore id \n") );
        return WHD_SEMAPHORE_ERROR;
    }

    result = osSemaphoreRelease(*semaphore);

    if ( (result == osOK) || (result == osErrorResource) )
    {
        return WHD_SUCCESS;
    }

    else if (result == osErrorParameter)
    {
        WPRINT_WHD_ERROR( ("semaphore set osErrorParameter \n") );
        return WHD_SEMAPHORE_ERROR;
    }
    return WHD_SEMAPHORE_ERROR;
}

/**
 * Deletes a semaphore
 *
 * WHD uses this function to delete a semaphore.
 *
 * @param semaphore         : Pointer to the semaphore handle
 *
 * @return whd_result_t : WHD_SUCCESS if semaphore was successfully deleted
 *                        : error if an error occurred
 *
 */
whd_result_t whd_rtos_deinit_semaphore(whd_semaphore_type_t *semaphore)
{
    osStatus_t result;
    if (*semaphore == NULL)
    {
        WPRINT_WHD_ERROR( (" null semaphore id \n") );
        return WHD_SEMAPHORE_ERROR;
    }
    result = osSemaphoreDelete(*semaphore);

    if (result != osOK)
        return WHD_SEMAPHORE_ERROR;

    return WHD_SUCCESS;
}

/**
 * Gets time in milliseconds since RTOS start
 *
 * @Note: since this is only 32 bits, it will roll over every 49 days, 17 hours.
 *
 * @returns Time in milliseconds since RTOS started.
 */
whd_time_t whd_rtos_get_time(void)
{
    /* Get Number of ticks per second */
    uint32_t tick_freq = osKernelGetTickFreq();

    /* Convert ticks count to time in milliseconds */
    return (osKernelGetTickCount() * (1000 / tick_freq) );

}

/**
 * Delay for a number of milliseconds
 *
 * @return whd_result_t : WHD_SUCCESS if delay was successful
 *                        : error if an error occurred
 *
 */
whd_result_t whd_rtos_delay_milliseconds(uint32_t num_ms)
{
    osStatus_t result;
    result = osDelay( (uint32_t )num_ms );

    if (result == osOK)
    {
        return WHD_SUCCESS;
    }
    else if (result == osErrorISR)
    {
        WPRINT_WHD_ERROR( (" osDelay ISR Error \n") );
        return WHD_RTOS_ERROR;
    }
    else if (result == osErrorParameter)
    {
        WPRINT_WHD_ERROR( (" osDelay osErrorParameter Error \n") );
        return WHD_BADARG;
    }
    else if (result == osErrorNoMemory)
    {
        WPRINT_WHD_ERROR( (" osDelay osErrorNoMemory Error \n") );
        return WHD_RTOS_ERROR;
    }
    return WHD_RTOS_ERROR;
}

