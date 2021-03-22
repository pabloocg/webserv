<!DOCTYPE html>
<html>
    <head lang="en">
		<meta charset="UTF-8">
        <title>Formulario</title>
	</head>
	<body>

	<h1>Hola <?php echo ($_GET['nombre']); ?>. </h1>
	<h2>Usted tiene <?php echo (int)$_GET['edad']; ?> años.</h2>
</body>
</html>