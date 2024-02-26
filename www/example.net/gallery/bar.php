<?php


header("Content-Type: text/plain");
// header("HTTP/1.1 200 OK");
header("HTTP/1.1 404 Not found");
echo "<div>HALLO FOO</div>";

// echo file_get_contents('php://input');

// var_dump($GLOBALS);

// var_dump($_SERVER);

// foreach($_REQUEST as $key => $value) {
//     echo "$key = $value";
// }

foreach($_POST as $key => $value) {
    echo "$key = $value";
}

// echo $_POST["texxt"] . "<br>";

// foreach($_SERVER as $key => $value) {
//     echo "$key = $value";
//     echo "<br>";
// }

?>