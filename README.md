Automação de bomba de poço utilizando ESP8266

No programa Arduino IDE faça as seguintes configurações arquivos>preferência e cole a seguinte url em urls adicionais:http://arduino.esp8266.com/stable/package_esp8266com_index.json

Após isso vá em ferramentas>placas>gerenciador de placas e procure por "ESP8266 community"

A partir disso vá na seleção de placas e selecione a opção "NodeMCU 1.0 (ESP-12E Module)"

em informações sensiveis.h coloce o ssid de sua rede e a senha, sua rede precisa estar em 2.4ghz caso contrário o dispositivo não será capaz de se conectar e realizar a função loop
