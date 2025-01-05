#include <iostream>
#include <logger.h>
#include <shared.h>

int main()
{
    Logger::set_depth(Logger::type::T_DEBUG);
    
    PathingStuff ps;

    //std::cout << "Current path: " << ps.get_base_path() << std::endl;
    //
    //const auto& gm = ps.get_own_magick();
    //const auto& gf = ps.get_own_ffmpeg();
    //const auto& g7 = ps.get_own_7zip();
    //
    //std::cout << "Magick: v='" << gm.version << "'; l='" << gm.download << "'" << std::endl;
    //std::cout << "FFMPEG: v='" << gf.version << "'; l='" << gf.download << "'" << std::endl;
    //std::cout << "7zip:   v='" << g7.version << "'; l='" << g7.download << "'" << std::endl;
    //if (g7.opt_dep) 
    //    std::cout << "7zr:     v='" << g7.opt_dep->version << "'; l='" << g7.opt_dep->download << "'" << std::endl;

//    std::cout << "Two examples of calls, one simple GET and other POST with GET params, header and post body" << std::endl;
//
//    const auto res = http::do_GET("http://v4.ipv6-test.com/api/myip.php");
//    const auto res2 = http::do_POST("https://echo.free.beeceptor.com",
//        { 
//            { "key1", "value1" },
//            { "key2", "value2" }
//        },        
//        http::request()
//            .set_headers({"User-Agent: TestingApp/1.0", "Accept: */*"})
//            .set_post_body(
//                { 
//                    { "key3", "value3" },
//                    { "key4", "value4" }
//                }
//            )
//    );
//
//    std::cout << res.body << std::endl;
//
//    for(const auto& i : res.head) {
//        std::cout << "HEAD: " << i << std::endl;
//    }
//
//    std::cout << res2.body << std::endl;
//
//    for(const auto& i : res2.head) {
//        std::cout << "HEAD: " << i << std::endl;
//    }

    return 0;
}