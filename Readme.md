# Linux 密碼修改 Daemon & Client 使用說明

這是一個使用 **PAM (Pluggable Authentication Modules)** 驗證的密碼修改系統，包括：  
**daemon**（負責處理密碼變更請求）  
**client**（用戶端，向 daemon 發送密碼修改請求）  

---

## **📌 目錄**
1. [編譯與執行](#編譯與執行)  
2. [如何使用](#如何使用)  
3. [停止 Daemon](#停止-Daemon)  
4. [其他命令](#其他命令)  


## 編譯與執行
### 編譯
在程式碼目錄下執行：
```sh
make
```
如果成功，會看到 `client` 和 `daemon` 兩個執行檔。

### 啟動 Daemon
Daemon 需要 **root 權限**，請執行：
```sh
sudo ./daemon &
```
確保 daemon 成功運行：
```sh
ps aux | grep daemon
```
如果 daemon 啟動成功，你應該會看到類似這樣的輸出：
```
root      3159  0.0  0.0   2756  2028 ?        Ss   23:32   0:00 ./daemon
```

## 如何使用
### 執行 Client 進行密碼修改
用戶可以執行：
```sh
./client
```
然後輸入：
```
請輸入您的使用者名稱: daetest
請輸入您的舊密碼: *****
請輸入您的新密碼: *****
```
如果密碼修改成功，會顯示：
```
密碼修改成功
```
如果密碼輸入錯誤，則會顯示：
```
身份驗證失敗
```

## 停止 Daemon
如果你想停止 daemon，可以執行：
```sh
sudo pkill daemon
```
然後確認 daemon 是否已經關閉：
```sh
ps aux | grep daemon
```
如果沒有輸出，表示 daemon 已經關閉。


## 其他命令
### 查看當前用戶
```sh
cat /etc/passwd
```
### 新增測試用戶
```sh
sudo useradd -m daetest
sudo passwd daetest
```

### 刪除測試用戶
```sh
sudo userdel -r daetest
```