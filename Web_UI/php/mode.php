<?php
$conn = require('***.php'); // Replace with database information PHP file name

// Check if mode is being set
if (isset($_GET['mode'])) {
    $mode = intval($_GET['mode']);
    if ($mode >= 0 && $mode <= 2) {
        $sql = "UPDATE mode_table SET mode = $mode WHERE id = 1";
        if ($conn->query($sql) === TRUE) {
            echo "Mode updated.";
        } else {
            echo "Error updating record: " . $conn->error;
        }
    } else {
        echo "Invalid mode.";
    }
} else {
    // Read current mode
    $sql = "SELECT mode FROM mode_table WHERE id = 1";
    $result = $conn->query($sql);

    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        echo $row['mode'];
    } else {
        echo "0"; // Default mode if no result is found
    }
}

$conn->close();
?>
