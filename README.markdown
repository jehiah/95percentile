95percentile
============

calculates the 95th percentile from a stream of integer/decimal values

    usage: 95percentile
        -p 0.95 (your percentile value)
        -i /path/to/input.json (optional; default is stdin)
        -d verbose output (to stderr)
        -v print version number
        -h this help

    cat response_times.log | ./95percentile
