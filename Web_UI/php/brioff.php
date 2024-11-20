<?php
$conn = require('***.php'); // Replace with database information PHP file name

// Check if brightness offset is being set
if (isset($_GET['brioff'])) {
    $brioff = floatval($_GET['brioff']);
    if ($brioff >= 0.5 && $brioff <= 10) {
        $sql = "UPDATE offset_table SET brioff = $brioff WHERE id = 1";
        if ($conn->query($sql) === TRUE) {
            echo "brioff updated to: $brioff";
        } else {
            echo "Error updating record: " . $conn->error;
        }
    } else {
        echo "Invalid brioff. It must be between 0.5 and 10.";
    }
} else {
    // Read current brightness
    $sql = "SELECT brioff FROM offset_table WHERE id = 1";
    $result = $conn->query($sql);

    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        echo $row['brioff']; 
    } else {
        echo "1.0"; // Default brioff value if no result is found
    }
}

$conn->close();
?>
