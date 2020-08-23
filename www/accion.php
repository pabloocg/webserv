<!DOCTYPE html>
<html>
    <head lang="en">
		<meta charset="UTF-8">
        <title>Formulario</title>
	</head>
	<body>

	<h1>Hola <?php echo htmlspecialchars($_POST['nombre']); ?>. </h1>
	<h2>Usted tiene <?php echo (int)$_POST['edad']; ?> años.</h2>
</body>
</html>