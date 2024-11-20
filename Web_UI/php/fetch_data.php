<?php
$conn = require('***.php'); // Replace with database information PHP file name

// Get the dropdown values
$dropdown1 = $_GET['dropdown1'];
$dropdown2 = $_GET['dropdown2'];
$date = isset($_GET['date']) ? $_GET['date'] : ''; 
$month = isset($_GET['month']) ? $_GET['month'] : ''; 

$data = array();

if ($dropdown1 == "Live") {
    $sql = "SELECT HOUR(timestamp) AS hour, AVG(temperature) AS temperature, AVG(humidity) AS humidity
            FROM sensor_data
            WHERE DATE(timestamp) = CURDATE()
            GROUP BY HOUR(timestamp)
            ORDER BY hour";
    
    $result = $conn->query($sql);
    while($row = $result->fetch_assoc()) {
        $data[] = $row;
    }

} elseif ($dropdown1 == "Daily" && $dropdown2 == "date") {
    $sql = "SELECT HOUR(timestamp) AS hour, AVG(temperature) AS temperature, AVG(humidity) AS humidity
            FROM sensor_data
            WHERE DATE(timestamp) = '$date'
            GROUP BY HOUR(timestamp)
            ORDER BY hour";
    
    $result = $conn->query($sql);
    while($row = $result->fetch_assoc()) {
        $data[] = $row;
    }

} elseif ($dropdown1 == "Monthly" && $dropdown2 == "month") {
    $sql = "SELECT DAY(timestamp) AS day, AVG(temperature) AS temperature, AVG(humidity) AS humidity
            FROM sensor_data
            WHERE MONTH(timestamp) = '$month'
            GROUP BY DAY(timestamp)
            ORDER BY day";
    
    $result = $conn->query($sql);
    while($row = $result->fetch_assoc()) {
        $data[] = $row;
    }

} elseif ($dropdown1 == "Yearly") {
    $sql = "SELECT DAY(timestamp) AS day, AVG(temperature) AS temperature, AVG(humidity) AS humidity
            FROM sensor_data
            WHERE YEAR(timestamp) = YEAR(CURDATE())
            GROUP BY DAY(timestamp)
            ORDER BY day";
    
    $result = $conn->query($sql);
    while($row = $result->fetch_assoc()) {
        $data[] = $row;
    }
}

// Return the data as JSON
echo json_encode($data);
$conn->close();
?>
