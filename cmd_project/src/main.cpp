#include <iostream>
#include <Lunaris/CURL/curl_cpp.h>

int main()
{
    std::cout << "Two examples of calls, one simple GET and other POST with GET params, header and post body" << std::endl;

    const auto res = http::do_GET("http://v4.ipv6-test.com/api/myip.php");
    const auto res2 = http::do_POST("https://echo.free.beeceptor.com",
        { 
            { "key1", "value1" },
            { "key2", "value2" }
        },        
        http::request()
            .set_headers({"User-Agent: TestingApp/1.0", "Accept: */*"})
            .set_post_body(
                { 
                    { "key3", "value3" },
                    { "key4", "value4" }
                }
            )
    );

    std::cout << res.body << std::endl;

    for(const auto& i : res.head) {
        std::cout << "HEAD: " << i << std::endl;
    }

    std::cout << res2.body << std::endl;

    for(const auto& i : res2.head) {
        std::cout << "HEAD: " << i << std::endl;
    }

    return 0;
}