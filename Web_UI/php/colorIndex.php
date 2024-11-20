<?php
$conn = require('***.php'); // Replace with database information PHP file name

if (isset($_GET['colorIndex'])) {
    $colorIndex = intval($_GET['colorIndex']);
    if ($colorIndex >= 0 && $colorIndex <= 6) {
        $sql = "UPDATE color_index_table SET colorIndex = $colorIndex WHERE id = 1"; // Assuming you're using ID 1
        if ($conn->query($sql) === TRUE) {
            echo "Color index updated.";
        } else {
            echo "Error updating record: " . $conn->error;
        }
    } else {
        echo "Invalid color index.";
    }
} else {
    // Read current colorIndex
    $sql = "SELECT colorIndex FROM color_index_table WHERE id = 1"; 
    $result = $conn->query($sql);

    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        echo $row['colorIndex'];
    } else {
        echo "0"; // Default colorIndex if no result is found
    }
}

$conn->close();
?>
