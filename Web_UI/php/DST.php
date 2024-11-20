<?php
$conn = require('***.php'); // Replace with database information PHP file name

if (isset($_GET['DST'])) {
    $DST = intval($_GET['DST']);
    if ($DST >= 0 && $DST <= 1) {
        $sql = "UPDATE DST_table SET DST = $DST WHERE id = 1";
        if ($conn->query($sql) === TRUE) {
            echo "DST updated.";
        } else {
            echo "Error updating record: " . $conn->error;
        }
    } else {
        echo "Invalid DST.";
    }
} else {

    // Read current DST
    $sql = "SELECT DST FROM DST_table WHERE id = 1";
    $result = $conn->query($sql);

    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        echo $row['DST'];
    } else {
        echo "0"; // Default DST if no result is found
    }
}

$conn->close();
?>
