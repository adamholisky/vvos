<?php
class Variable {
	public $name = "";
	public $type = "";
	public $printf_char = "";
	public $printf_var = "";

	public function __construct( $_name, $_type, $_printf_char, $_printf_var ) {
		$this->name = str_replace( ";", "", $_name);
		$this->type = str_replace( ";", "", $_type);
		$this->printf_char = str_replace( ";", "", $_printf_char);
		$this->printf_var = str_replace( ";", "", $_printf_var);
	}
}

$separator = "\r\n";
$parsed_name = "name_unknown";
$vars = array();

if( $argc != 2 ) {
	echo "Usage: php s2debugf.php <file name>\n";
	exit;
}

$f = file_get_contents( $argv[1] );

if( $f == false ) {
	echo "File cannot be opened.\n";
	exit;
}

$line = strtok($f, $separator);

while ($line !== false) {
    $parts = preg_split('/\s+/', $line);

	switch( $parts[0] ) {
		case "typedef":
			break;
		case "}":
			$parsed_name = $parts[1];
			break;
		default:
			$type_element = 1;
			$var_element = 2;
			$name = '';
			$element = '';
			$printf_char = '';
			$printf_var = '';

			if( $parts[2] == "*" ) {
				$type_element = $parts[1] . " *";
				$var_element = $parts[3];
			} else {
				$type_element = $parts[1];
				$var_element = $parts[2];
			}

			switch( $type_element ) {
				case "char *":
					$printf_char = "%s";
					$printf_var = $var_element;
					break;
				case "uint32_t":
					$printf_char = "%d (0x%x)";
					$printf_var = $var_element . ", " . $var_element;
					break;
			}

			echo $type_element . "\n";
			echo $var_element . "\n";
			echo $printf_char . "\n";
			echo $printf_var . "\n";

			$vars[] = new Variable( $var_element, $type_element, $printf_char, $printf_var );
	}

    $line = strtok( $separator );
}

echo 'printf( "' . str_replace( ";", "", $parsed_name ) . ' readout:\n" );' . "\n";

foreach( $vars as $var ) {
	echo 'printf( "    ' . $var->name . ': ' . $var->printf_char . '\n", ' . $var->printf_var . ');' . "\n";
}
