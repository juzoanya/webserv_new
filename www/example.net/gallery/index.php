<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Image Upload</title>
</head>
<body>
    <h2>Image Upload</h2>
    <form action="gallery/upload.php" method="post" enctype="multipart/form-data">
        <label for="fileToUpload">Choose Pictures to Upload</label>
        <input type="file" name="fileToUpload" id="fileToUpload">
        <input type="submit" name="submit" value="Upload Images">
    </form>
    <br>
    <h2>Image Gallery</h2>
    <div class="gallery">
        <?php
            $directory = "uploads/";
            $files = glob($directory . "*.{jpg,jpeg,png,gif}", GLOB_BRACE);
            foreach($files as $image) {
                echo '<img src="'.$image.'" width="200px" />';
            }
        ?>
    </div>
</body>
</html>