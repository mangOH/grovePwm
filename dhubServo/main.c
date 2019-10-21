#include "legato.h"
#include "interfaces.h"

static void HandleAngleSetpoint
(
    double timestamp,
    double angle,
    void* contextPtr
)
{
    size_t index = (size_t)contextPtr;
    LE_ASSERT((index >= 1) && (index <= 16));

    // If "Not a Number" is received, don't change the angle.
    if (isnan(angle))
    {
        return;
    }

    // Clamp the angle range from 0 to 180.
    if (angle > 180.0)
    {
        angle = 180.0;
    }
    else if (angle < 0.0)
    {
        angle = 0.0;
    }

    servo_SetAngle(index, (int)angle);
}

COMPONENT_INIT
{
    le_result_t result;

    LE_INFO("Initializing Grove PWM Servo Motor Data Hub Interface.");

    // Create an array of 16 servo control outputs from the Data Hub and register push
    // handlers for them.
    for (size_t i = 1; i <= 16; i++)
    {
        char resourcePath[32];
        int len = snprintf(resourcePath, sizeof(resourcePath), "%d/angle", i);
        LE_ASSERT(len < sizeof(resourcePath));
        result = io_CreateOutput(resourcePath, IO_DATA_TYPE_NUMERIC, resourcePath);
        LE_ASSERT(result == LE_OK);

        // Register for notification of updates to the counter value.
        // Pass the index of the PWM channel to the call-back as the contextPtr.
        io_AddNumericPushHandler(resourcePath, HandleAngleSetpoint, (void*)i);
    }
}
