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

void serialEvent()
{
    while(Serial.available())
    {
        char c = Serial.read();
        Serial1.write(c);
        Serial1.flush();
    }
}

void serialEvent1()
{
    while(Serial1.available())
    {
        char c = Serial1.read();
        Serial.write(c);
        Serial.flush();
    }
}

void loop()
{
    //Serial1.println("test");
    //delay(100);
}