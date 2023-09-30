# projeto-tcc-software
Projeto de tcc - UTFPR

Antes de enviar o código, algumas coisas precisam ser configuradas no Arduino IDE. Se esta é sua primeira vez com o ESP32-CAM no Arduino IDE você precisa configurar as bibliotecas de hardware do ESP32, aprenda a conectar e testar seguindo este tutorial ESP32-CAM no Arduino IDE

Existem três bibliotecas que precisam ser instaladas. O TFT_eSPI pode ser facilmente instalado a partir do gerenciador de bibliotecas IDE (Ferramentas > Gerenciar Bibliotecas) procurando por TFT_eSPI. As bibliotecas TFT_eFEX e ESPAsyncWebserver precisam ser instaladas baixando as bibliotecas usando o link 'Download ZIP' e no IDE instalando-as com Sketch > Incluir Biblioteca > Adicionar Biblioteca .ZIP.

A biblioteca TFT_eSPI precisa ser configurada para funcionar com o painel TFT ST7735S. Copie o conteúdo do arquivo User_Setup.h para o arquivo de biblioteca recém-instalado User_Setup.h encontrado em Documentos > Arduino > bibliotecas > TFT_eSPI (.pio/libdeps/esp32cam/TFT_eSPI/User_Setup.h).

Caso queira utilizar a animação de contagem regressiva, as imagens para isso precisam ser carregadas na memória do ESP32. Para fazer isso siga as instruções para instalar o uploader de pasta de dados aqui: ESP32 Data Folder Uploader . Lembre-se de que se você alterar o esquema de partição no IDE, esses dados serão sobrescritos.