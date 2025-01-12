/**
 * brief: iomanager简单测试
 */
#include "bifang.h"

RootLogger();

int sock = 0;

void io_test()
{
    sock = socket(AF_INET, SOCK_STREAM, 0);
    bifang::hook_disable();

    log_info << "test_fiber sock=" << sock;

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9090); //80
    inet_pton(AF_INET, "20.200.23.154", &addr.sin_addr.s_addr); // 123.57.17.199

    if (!connect(sock, (const sockaddr*)&addr, sizeof(addr)))
    {
        log_info << "connect sucessful";
    }
    else if (errno == EINPROGRESS) // 表示连接进行中
    {
        log_info << "add event errno=" << errno << ", strerror:" << strerror(errno);
        bifang::IOManager::getThis()->add(sock, bifang::IOContext::READ,
            [](){
                    log_info << "read callback";
                });
        log_info << "read callback";
        bifang::IOManager::getThis()->add(sock, bifang::IOContext::WRITE,
            [](){
                    log_info << "write callback";
                    bifang::IOManager::getThis()->cancel(sock, bifang::IOContext::READ);
                    close(sock);
                });
    }
}

void test()
{
    bifang::IOManager iom(2, true, "test");
    iom.schedule(&io_test);
}

bifang::Timer::ptr g_timer;
void timer_test()
{
    bifang::IOManager iom(1);
    g_timer = iom.addTimer(800,
        [&](){
                static int count = 0;
                log_info << "timer execute count=" << count;
                if (++count == 5)
                    g_timer->reset(100);
                if (count > 30)
                    g_timer->cancel();
            }, true);
}

int main(int argc, char* argv[])
{
    test();
    timer_test();
    
    return 0;
}
