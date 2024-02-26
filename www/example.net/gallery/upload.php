<?php
$targetDir = "uploads/";
$uploadOk = 1;
$allowedFormats = array("jpg", "jpeg", "png", "gif");


echo "BROOO " . $_SERVER["REQUEST_METHOD"] . " und size: " . $_SERVER['CONTENT_LENGTH'];;

foreach($_POST as $key => $value) {
    echo "$key = $value";
}

if(isset($_POST["submit"])) {
    $files = $_FILES['files'];

    foreach($files['name'] as $key=>$name) {
        $targetFile = $targetDir . basename($files['name'][$key]);
        $imageFileType = strtolower(pathinfo($targetFile, PATHINFO_EXTENSION));

        // Überprüfen, ob es sich um ein echtes Bild handelt
        $check = getimagesize($files["tmp_name"][$key]);
        if($check === false) {
            echo "File is not an image.";
            $uploadOk = 0;
        }

        // Überprüfen, ob die Datei bereits existiert
        if (file_exists($targetFile)) {
            echo "Sorry, file already exists.";
            $uploadOk = 0;
        }

        // Überprüfen, ob die Dateigröße akzeptabel ist (hier: 5 MB)
        if ($files["size"][$key] > 5000000) {
            echo "Sorry, your file is too large.";
            $uploadOk = 0;
        }

        // Erlaubte Dateiformate festlegen
        if(!in_array($imageFileType, $allowedFormats)) {
            echo "Sorry, only JPG, JPEG, PNG, GIF files are allowed.";
            $uploadOk = 0;
        }

        // Alle Überprüfungen bestanden, versuche das Bild hochzuladen
        if ($uploadOk == 0) {
            echo "Sorry, your file was not uploaded.";
        } else {
            if (move_uploaded_file($files["tmp_name"][$key], $targetFile)) {
                echo "The file ". htmlspecialchars(basename($files["name"][$key])). " has been uploaded.";
            } else {
                echo "Sorry, there was an error uploading your file.";
            }
        }
    }
} else {
    header("Header-Submit: NOT SET");
}
header("HTTP/1.1 304 See Other");
header("Location: /gallery/index.php");

?>