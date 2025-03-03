#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/password_daemon.socket"
#define BUFFER_SIZE 1024

int main()
{
    char username[50], old_password[50], new_password[50];

    printf("請輸入您的使用者名稱: ");
    scanf("%s", username);

    printf("請輸入您的舊密碼: ");
    system("stty -echo"); // 隱藏輸入
    scanf("%s", old_password);
    system("stty echo");

    printf("\n請輸入您的新密碼: ");
    system("stty -echo");
    scanf("%s", new_password);
    system("stty echo");

    // 創建 socket 並連接 Daemon
    int sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        exit(1);
    }

    // 發送資料
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%s %s %s", username, old_password, new_password);
    write(sock_fd, buffer, strlen(buffer));

    // 讀取回應
    memset(buffer, 0, BUFFER_SIZE);
    read(sock_fd, buffer, BUFFER_SIZE);
    printf("\n伺服器回應: %s\n", buffer);

    close(sock_fd);
    return 0;
}
