#define UART_BUFFER_SIZE 5

unsigned char readBuf[UART_BUFFER_SIZE];

void setup()
{
    Serial.begin(115200);
    while(!Serial)
    {
        // 시리얼 포트 연결까지 대기 
    }

    Serial1.begin(115200);
    while(!Serial1)
    {
        // 시리얼 포트 연결까지 대기 
    }

    Serial.println("UART Test Program");
    Serial.flush();
}

bool decode_uart0()
{
    for(int i=UART_BUFFER_SIZE-3; i>=0; i--)
    {
        if(readBuf[i] == '#' && readBuf[i+2] == '*')
        {
            if(readBuf[i+1] == 'C')
            {
                Serial.println("Decode Success! : C");
                return true;
            }
            else if(readBuf[i+1] == 'D')
            {
                Serial.println("Decode Success! : D");
                return true;
            }
            else
            {
                Serial.println("Skrrrrrrrrrrrrrrt : NO command");
                return false;
            }
        }
        else
        {
            return false;
        }
    }

}

void serialEvent()
{
    while(Serial.available())
    {
        for(int i=0; i<UART_BUFFER_SIZE; i++)
        {
            readBuf[i] = readBuf[i+1];
        }

        uint8_t receivedByte = Serial.read();
        readBuf[UART_BUFFER_SIZE-1] = receivedByte;

        decode_uart0();
        /*
        for(int i=0; i<UART_BUFFER_SIZE; i++)
        {
            Serial.print(readBuf[i], HEX);
        }

        Serial.println("");
        //char c = Serial.read();
        //Serial1.print(c);
        //Serial.println("received");
        */
    }
}

void loop()
{
    //Serial1.println("test");
    //delay(100);

}