#include <bsec_interface.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>

LOG_MODULE_REGISTER(bme688, LOG_LEVEL_DBG);

static struct k_thread thread_data;
#define BME688_STACK_SIZE 4096
K_THREAD_STACK_DEFINE(bme688_stack, BME688_STACK_SIZE);
static const struct device *const gasometer = DEVICE_DT_GET(DT_NODELABEL(bme688));

static void bme688_task(void *, void *, void *);

void bme688_init()
{
    if (!device_is_ready(gasometer)) {
        LOG_ERR("Device is not ready");
    }
	/* Create the thread */
	k_thread_create(&thread_data, bme688_stack,
                    BME688_STACK_SIZE,
                    &bme688_task,
				    NULL,    /* p1 */
				    NULL,    /* p2 */
				    NULL,    /* p3 */
				    5,       /* Zephyr priority */
				    0,          /* thread options */
				    K_NO_WAIT); /* start immediately */
	/* Set alias for thread */
	k_thread_name_set(&thread_data, "bme688");
}

static void bme688_task(void *, void *, void *)
{
    while(1) {
        k_sleep(K_SECONDS(1));
        LOG_INF("hello");
    }
}
