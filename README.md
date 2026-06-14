# 🦴 Disk_Devourer

**Um exploit educacional em C++ que escaneia o sistema e inunda discos rígidos com milhares de arquivos `.txt`.**

⚠️ **ATENÇÃO:** Este projeto é **APENAS PARA FINS EDUCACIONAIS**. O autor não se responsabiliza por qualquer uso indevido. **NÃO execute em sistemas sem autorização.**

## 🎯 Funcionalidades

- ✅ Escaneia automaticamente todas as unidades fixas do sistema (C:\, D:\, etc.)
- ✅ Coleta informações do sistema (nome do computador, usuário, versão do Windows)
- ✅ Cria até **50.000 arquivos** com 500 linhas cada (PCFUCKED)
- ✅ Cria um log (`PCFUCKED_LOG.txt`) com os dados coletados
- ✅ Roda em segundo plano (console escondido)

## ⚙️ Como Compilar

```bash
g++ -static -std=c++17 -o Disk_Devourer.exe Disk_Devourer.cpp -lgdi32 -lwinmm -lole32 -luuid -luser32 -lshlwapi
