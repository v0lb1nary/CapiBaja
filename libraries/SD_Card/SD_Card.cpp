// Implementação 
// Define como as funções trabalham e quais operações realizam.
#include "SD_Card.h" // Importa o cabeçalho para garantir que as declarações das funções sejam reconhecidas.

// Definição dos novos pinos SPI para o SD Card
#define SD_SCK   14  // Clock
#define SD_MISO  12  // Entrada de dados
#define SD_MOSI  13  // Saída de dados
#define SD_CS    27   // Chip Select (ajuste conforme necessário)

// ------------------------------------------------------------

void SD_Card::setup(){

  while (!Serial) {
    delay(10);
  }

  Serial.println("Inicializando cartao SD com pinos SPI personalizados...");

  // Inicializa o barramento SPI com os novos pinos
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  if (!SD.begin(SD_CS, SPI)) {
    Serial.println("Falha na inicializaccao do SD!");
    return;
  }
  
  Serial.println("Cartao SD pronto!");
  
  uint8_t cardType = SD.cardType();  
   
  if(cardType == CARD_NONE){
    Serial.println("Nenhum cartao SD conectado");
    return;
  }

  Serial.print("Tipo do cartao SD: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("DESCONHECIDO");
  }

}

// ------------------------------------------------------------

void SD_Card::listaDiretorio(const char * dirname, uint8_t levels){
  Serial.printf("Listando diretórios: %s\n", dirname);

  File root = SD.open(dirname);
  if(!root){
    Serial.println("Falha ao abrir diretório");
    return;
  }
  if(!root.isDirectory()){
    Serial.println("Nao é um diretório");
    return;
  }

  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()){
      Serial.print("  PASTA   : ");
      Serial.println(file.name());
      if(levels){
        listaDiretorio(file.name(), levels -1);
      }
    } else {
      Serial.print("  ARQUIVO: ");
      Serial.print(file.name());
      Serial.print("  TAMANHO: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

// ------------------------------------------------------------

void SD_Card::criandoDiretorio(const char * path){
  Serial.printf("Criando diretório: %s\n", path);
  if(SD.mkdir(path)){
    Serial.println("Diretório criado");
  } else {
    Serial.println("Falha ao criar diretório");
  }
}

// ------------------------------------------------------------

void SD_Card::apagandoDiretorio(const char * path){
  Serial.printf("Removendo diretório: %s\n", path);
  if(SD.rmdir(path)){
    Serial.println("Diretório removido");
  } else {
    Serial.println("Falha ao remover diretório");
  }
}

// ------------------------------------------------------------

void SD_Card::lendoArquivo(const char * path){
  Serial.printf("Lendo arquivo: %s\n", path);

  File file = SD.open(path);
  if(!file){
    Serial.println("Falha ao abrir arquivo para leitura");
    return;
  }

  Serial.print("Conteúdo do arquivo: \n");
  while(file.available()){
    Serial.write(file.read());
  }
  file.close();
}

// ------------------------------------------------------------

void SD_Card::criandoArquivo(const char * path, const char * message){
  Serial.printf("Escrevendo arquivo: %s\n", path);

  File file = SD.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Falha ao abrir arquivo para escrita");
    return;
  }
  if(file.print(message)){
    file.print("\n");
    Serial.println("Arquivo escrito");
  } else {
    Serial.println("Falha na escrita");
  }
  file.close();
}

// ------------------------------------------------------------

void SD_Card::incrementandoArquivo(const char * path, const char * message){
  //Serial.printf("Adicionando ao arquivo: %s\n", path);

  File file = SD.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Falha ao abrir arquivo para adicionar conteúdo");
    return;
  }
  if(file.print(message)){
      Serial.println("Mensagem adicionada");
      file.print("\n"); // Pula linhas CSV
  } else {
    Serial.println("Falha ao adicionar conteúdo");
  }
  file.close();
}

// ------------------------------------------------------------

void SD_Card::renomeandoArquivo(const char * path1, const char * path2){
  Serial.printf("Renomeando arquivo de %s para %s\n", path1, path2);
  if (SD.rename(path1, path2)) {
    Serial.println("Arquivo renomeado");
  } else {
    Serial.println("Falha ao renomear arquivo");
  }
}

// ------------------------------------------------------------

void SD_Card::deletandoArquivo(const char * path){
  Serial.printf("Deletando arquivo: %s\n", path);
  if(SD.remove(path)){
    Serial.println("Arquivo deletado");
  } else {
    Serial.println("Falha ao deletar arquivo");
  }
}

// ------------------------------------------------------------

void SD_Card::testeGeral(const char * path){
  File file = SD.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if(file){
    len = file.size();
    size_t flen = len;
    start = millis();
    while(len){
      size_t toRead = len;
      if(toRead > 512){
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    Serial.printf("%u bytes lidos em %u ms\n", flen, end);
    file.close();
  } else {
    Serial.println("Falha ao abrir arquivo para leitura");
  }

  file = SD.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Falha ao abrir arquivo para escrita");
    return;
  }

  size_t i;
  start = millis();
  for(i=0; i<2048; i++){
    file.write(buf, 512);
  }
  end = millis() - start;
  Serial.printf("%u bytes escritos em %u ms\n", 2048 * 512, end);
  file.close();
}

// ------------------------------------------------------------

void SD_Card::armazenamentoGeral(){
    Serial.printf("Espaco total: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Espaco usado: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}
