<!DOCTYPE html>
<html>
<head>
    <title>Interaktive PHP-Datei</title>
</head>
<body>

<?php
    // Funktion, um die Tageszeit zu bestimmen
    function get_time_of_day() {
        date_default_timezone_set('Europe/Berlin');
        $hour = date('H');
        if ($hour >= 5 && $hour < 12) {
            return "Morgen";
        } elseif ($hour >= 12 && $hour < 18) {
            return "Tag";
        } else {
            return "Abend";
        }
    }

    // Begrüßungsnachricht basierend auf der Tageszeit
    $time_of_day = get_time_of_day();
    echo "<h1>Guten $time_of_day!</h1>";
    echo "<p>Die aktuelle Uhrzeit ist: " . date('H:i:s') . "</p>";
    echo strval(getmygid());
?>

</body>
</html>