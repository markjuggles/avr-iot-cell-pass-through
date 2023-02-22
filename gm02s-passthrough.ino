// Program: gm02s-passthrough.ino
// Purpose: Allow access to the GM02S via the external UART pins (Serial2).  
//
// This is derived from Microchip AVR-IoT Cell Mini examples.  
// Microchip license restrictions apply.

// Compile+Upload with Ctrl-Shift-U

#include <Arduino.h>
#include <log.h>
#include <sequans_controller.h>

void debugBridgeUpdate(void);

// ------------------------------ DEBUG BRIDGE ----------------------------- //

#ifdef __AVR_AVR128DB48__ // MINI

#define SerialDebug Serial3   // USB Console

#else
#ifdef __AVR_AVR128DB64__ // Non-Mini

#define SerialDebug Serial5

#else
#error "INCOMPATIBLE_DEVICE_SELECTED"
#endif
#endif

#define BACKSPACE 8

#define INPUT_BUFFER_SIZE    256

void setup()
{
    Log.begin(115200);
    SerialDebug.println("The external UART is connected to the GM02S");

    Serial2.swap(1);
    Serial2.begin(115200);

    SequansController.begin();

    // The controller will set initialized to true if it receives the +SYSSTART string so regenerate it.
    if(SequansController.isInitialized() == true)
    {
      Serial2.println("+SYSSTART");
    }
    
}

void loop()
{
    static uint8_t data;
    static char uartBuffer[INPUT_BUFFER_SIZE];
    static uint8_t uartIndex = 0;

    // Read input from the external facing UART (Serial2).
    if (Serial2.available())
    {
        data = Serial2.read();

        // Process a backspace, carrige return, or ordinary character.
        if (data == BACKSPACE)
        {
          if (uartIndex > 0)
          {
            --uartIndex;            
          }
        }
        else
        if (data == '\r')
        {
          uartBuffer[uartIndex++] = '\r';
          uartBuffer[uartIndex++] = '\0';
          SequansController.writeBytes((const uint8_t*)uartBuffer, strlen(uartBuffer));
          uartIndex = 0;
        }
        else
        {
          if(uartIndex < (INPUT_BUFFER_SIZE-2))
          {
            uartBuffer[uartIndex++] = data;
          }
        }
    }

    if (SequansController.isRxReady()) 
    {
        // Display data from the GM02S to the external UART and the USB UART.
        data = SequansController.readByte();
        SerialDebug.write(data);
        Serial2.write(data);
    }
}
