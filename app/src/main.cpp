// #include <argparse.hpp>
//#include <spdlog/spdlog.h>
//#include <spdlog/sinks/stdout_sinks.h>

int main(int argc, char *argv[])
{
    // auto parser = argparse::ArgumentParser("Multilevel Circuit Partitioning");

    // parser.add_argument("square")
    //   .help("display the square of a given integer")
    //   .action([](const std::string& value) { return std::stoi(value); });

    // parser.add_argument("-v", "--verbose")
    //   .help("increase output verbosity")
    //   .default_value(false)
    //   .implicit_value(true);

    // parser.add_argument("-vv", "--very-verbose")
    //   .help("increase output verbosity to DEBUG")
    //   .default_value(false)
    //   .implicit_value(true);

    // // try
    // // {
    //     parser.parse_args(argc, argv);
    // }
    // catch (const std::runtime_error& err)
    // {
    //     std::cout << err.what() << std::endl;
    //     std::cout << parser;
    //     exit(0);
    // }

    // // auto console = spdlog::stdout_logger_mt("console");
    // // console->set_pattern("[%H:%M:%S %z] [%n] [thread %t] %v");
    // // console->set_level(spdlog::level::warn);

    // // if (parser["--verbose"] == true)
    // // {
    // //     console->set_level(spdlog::level::info);
    // // }
    // // if (parser["--very-verbose"] == true)
    // // {
    // //     console->set_level(spdlog::level::debug);
    // // }

    // // console->info("program begin...");

    // auto input = parser.get<int>("square");
    // std::cout << (input * input) << std::endl;
    // console->debug("The square of {} is {}.", input, (input*input));

    // console->info("program end...");
    return 0;
}
