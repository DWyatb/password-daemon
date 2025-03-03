#define _GNU_SOURCE
#include <crypt.h>
#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/password_daemon.socket"
#define BUFFER_SIZE 1024

// PAM 會使用這個回調函數來讀取密碼

static int pam_conv_func(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr)
{
    struct pam_response *responses = malloc(num_msg * sizeof(struct pam_response));
    if (!responses)
        return PAM_CONV_ERR;

    char *password = (char *)appdata_ptr;
    for (int i = 0; i < num_msg; i++) {
        responses[i].resp = strdup(password);
        responses[i].resp_retcode = 0;
    }
    *resp = responses;
    return PAM_SUCCESS;
}

// 使用 PAM 來驗證用戶的舊密碼
int authenticate_user(const char *username, const char *password)
{
    struct pam_conv pam_conversation = {pam_conv_func, (void *)password};
    pam_handle_t *pamh = NULL;
    int ret = pam_start("passwd", username, &pam_conversation, &pamh);

    if (ret == PAM_SUCCESS)
        ret = pam_authenticate(pamh, 0);
    if (ret == PAM_SUCCESS)
        ret = pam_acct_mgmt(pamh, 0);

    pam_end(pamh, ret);
    return ret == PAM_SUCCESS;
}

// 更新密碼
int change_password(const char *username, const char *new_password)
{
    char command[BUFFER_SIZE];
    snprintf(command, sizeof(command), "echo \"%s:%s\" | chpasswd", username, new_password);
    return system(command);
}

// Daemon 主函數
void run_daemon()
{
    int server_fd, client_fd;
    struct sockaddr_un server_addr;

    // 創建 UNIX domain socket
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // 設置 socket 地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);
    unlink(SOCKET_PATH);

    // 綁定 socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(1);
    }

    chmod(SOCKET_PATH, 0777);

    // 監聽連線
    if (listen(server_fd, 5) == -1) {
        perror("listen");
        exit(1);
    }

    printf("Daemon 正在運行，等待請求...\n");

    while (1) {
        char buffer[BUFFER_SIZE];
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        memset(buffer, 0, BUFFER_SIZE);
        read(client_fd, buffer, BUFFER_SIZE);

        char username[50], old_password[50], new_password[50];
        sscanf(buffer, "%s %s %s", username, old_password, new_password);

        if (authenticate_user(username, old_password)) {
            if (change_password(username, new_password) == 0) {
                write(client_fd, "密碼修改成功", 20);
            } else {
                write(client_fd, "密碼修改失敗", 20);
            }
        } else {
            write(client_fd, "身份驗證失敗", 20);
        }

        close(client_fd);
    }

    close(server_fd);
    unlink(SOCKET_PATH);
}

int main()
{
    if (fork() > 0) {
        exit(0); // 父進程退出，子進程變為 Daemon
    }
    setsid();
    chdir("/");
    run_daemon();
    return 0;
}
