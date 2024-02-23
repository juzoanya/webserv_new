<?php
// Check if the form was submitted
if ($_SERVER["REQUEST_METHOD"] == "POST" && !empty($_FILES["images"])) {
    // Define the target directory
    $targetDir = "gallery/";

    // If the target directory doesn't exist, create it
    if (!file_exists($targetDir)) {
        mkdir($targetDir, 0777, true);
    }

    // Loop through each file
    $uploadedFiles = [];
    foreach ($_FILES["images"]["tmp_name"] as $key => $tmp_name) {
        $fileName = basename($_FILES["images"]["name"][$key]);
        $targetFilePath = $targetDir . $fileName;
        $fileType = pathinfo($targetFilePath, PATHINFO_EXTENSION);

        // Check if the file is an image
        $check = getimagesize($_FILES["images"]["tmp_name"][$key]);
        if ($check !== false) {
            // Allow only certain file formats
            $allowedFormats = array("jpg", "jpeg", "png", "gif");
            if (in_array($fileType, $allowedFormats)) {
                // Upload the file
                if (move_uploaded_file($_FILES["images"]["tmp_name"][$key], $targetFilePath)) {
                    $uploadedFiles[] = $fileName;
                } else {
                    echo "Error uploading file: " . $_FILES["images"]["error"][$key];
                }
            } else {
                echo "Only JPG, JPEG, PNG, and GIF files are allowed.";
            }
        } else {
            echo "File is not an image.";
        }
    }

    // Display success message
    if (!empty($uploadedFiles)) {
        echo "Files uploaded successfully: " . implode(", ", $uploadedFiles);
    }
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Image Uploader</title>
</head>
<body>
    <form action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" method="post" enctype="multipart/form-data">
        <input type="file" name="images[]" multiple accept="image/*">
        <input type="submit" value="Upload">
    </form>
</body>
</html>
