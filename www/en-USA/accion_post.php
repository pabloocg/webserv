<?php  
//var_dump($_POST);
parse_str(file_get_contents("php://input"), $_POST);
//var_dump($_POST);
?>
<!DOCTYPE html>
<html>
    <head lang="en">
		<meta charset="UTF-8">
        <title>Formulario</title>
	</head>
	<body>

	<h1>Hola <?php echo ($_POST['nombre']); ?>. </h1>
	<h2>Usted tiene <?php echo (int)$_POST['edad']; ?> años.</h2>
</body>
</html>