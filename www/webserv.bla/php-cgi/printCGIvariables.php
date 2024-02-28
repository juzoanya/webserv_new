<?php
    header('content-type: text/plain');
    echo "QUERY_STRING = "         , $_SERVER["QUERY_STRING"]       , "\n";
    echo "REQUEST_METHOD = "       , $_SERVER["REQUEST_METHOD"]     , "\n";
    echo "CONTENT_TYPE = "         , $_SERVER["CONTENT_TYPE"]       , "\n";
    echo "CONTENT_LENGTH = "       , $_SERVER["CONTENT_LENGTH"]     , "\n";
    echo "SCRIPT_FILENAME = "      , $_SERVER["SCRIPT_FILENAME"]    , "\n";
    echo "SCRIPT_NAME = "          , $_SERVER["SCRIPT_NAME"]        , "\n";
    echo "PATH_INFO = "            , $_SERVER["PATH_INFO"]          , "\n";
    echo "PATH_TRANSLATED = "      , $_SERVER["PATH_TRANSLATED"]    , "\n";
    echo "REQUEST_URI = "          , $_SERVER["REQUEST_URI"]        , "\n";
    echo "SERVER_PROTOCOL = "      , $_SERVER["SERVER_PROTOCOL"]    , "\n";
    echo "GATEWAY_INTERFACE = "    , $_SERVER["GATEWAY_INTERFACE"]  , "\n";
    echo "SERVER_SOFTWARE = "      , $_SERVER["SERVER_SOFTWARE"]    , "\n";
    echo "REMOTE_ADDR = "          , $_SERVER["REMOTE_ADDR"]        , "\n";
    echo "REMOTE_PORT = "          , $_SERVER["REMOTE_PORT"]        , "\n";
    echo "SERVER_ADDR = "          , $_SERVER["SERVER_ADDR"]        , "\n";
    echo "SERVER_PORT = "          , $_SERVER["SERVER_PORT"]        , "\n";
    echo "SERVER_NAME = "          , $_SERVER["SERVER_NAME"]        , "\n";
    echo "REDIRECT_STATUS = "      , $_SERVER["REDIRECT_STATUS"]    , "\n";
?>






















