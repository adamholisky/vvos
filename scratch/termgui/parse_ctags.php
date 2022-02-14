<?php
class Variable {
	public $name = "";
	public $type = "";
	public $scope = "";
	public $func_name = "";

	public function __construct( $_name, $_type, $_scope, $_func_name ) {
		$this->name = $_name;
		$this->type = $_type;
		$this->scope = $_scope;
		$this->func_name = $_func_name;
	}

	static public function find_var_where( $vars, $field, $var ) {
		foreach( $vars as $var ) {
			$compare_to = '';

			switch( $field ) {
				case 'name':
					$compare_to = $var->name;
					break;
				case 'type':
					$compare_to = $var->type;
					break;
				case 'scope':
					$compare_to = $var->scope;
					break;
				case 'func_name':
					$compare_to = $var->func_name;
					break;
			}

			if( $var == $compare_to ) {
				echo "found!\n";
				return $var;
			}
		}
	}
}

$separator = "\r\n";
$parsed_name = "name_unknown";
$vars_local = array();
$vars_types = array();
$vars_members = array();
$vars_structs = array();

$defines = "";
$globals = "";
$prototypes = "";
$code = "";

if( $argc != 2 ) {
	echo "Usage: php parse_ctags.php <file name>\n";
	exit;
}

$f = file_get_contents( "tags" );

if( $f == false ) {
	echo "File cannot be opened.\n";
	exit;
}

$line = strtok($f, $separator);

while ($line !== false) {
	if( $line[0] !== '!' ) {
		$parts = preg_split('/\t+/', $line);

		//typeref:typename:int
		if( $parts[3] == 't' ) {
			$type_split = preg_split( '/:/', $parts[4] );
			$type = $type_split[2];
		} else if( $parts[3] == 's' ) {
			$type = 'struct';
		} else if( $parts[3] == 'v' ) {
			$type_split = preg_split( '/:/', $parts[4] );
			$type = $type_split[2];
		} else {
			$type_split = preg_split( '/:/', $parts[5] );
			$type = $type_split[2];
		}
		
	
		//function:draw_title_line
		if( $parts[3] == 'v' ) {
			$func_name = "global";
		} else if( $parts[3] == 's' ) {
			$func_name = "struct";
		} else {
			$func_name_split = preg_split( '/:/', $parts[4] );
			$func_name = $func_name_split[1];
		}
		
		
		switch( $parts[3] ) {
			case 'l':
			case 'v':
				$vars_local[] = new Variable( $parts[0], $type, $parts[3], $func_name );
				break;
			case 'm':
				$vars_members[] = new Variable( $parts[0], $type, $parts[3], $func_name );
				break;
			case 's':
				$vars_structs[] = new Variable( $parts[0], $type, $parts[3], $func_name );
				break;
			case 't':
				$vars_types[] = new Variable( $parts[0], $type, $parts[3], $func_name );
				break;
			default:
				echo "*****UNHANDLED TYPE: " . $parts[3] . "\n";
		}
	}
   
    $line = strtok( $separator );
}

/* foreach( $vars as $var ) {
	echo $var->name . "\n  " . $var->type . "\n  " . $var->scope . "\n  " . $var->func_name  . "\n";
} */

$defines += "#include \"creflect.h\"\n";
$defines =+ "\n";

foreach( $vars_structs as $var ) {
	foreach( $vars_types as $struct_proc_name ) {
		if( $struct_proc_name->type == $var->name ) {
			$struct_name = $struct_proc_name->name;
		}
	}

	$code =+ "void DISPLAY_" . strtoupper($struct_name) . "( " . $struct_name . " *var ) {\n";
	
	foreach( $vars_members as $var_m ) {
		if( $var_m->func_name == $var->name  ) {
			$code =+ "\tDISPLAY_" . get_var_type_upper( $var_m->type ) . "( \"" . $var_m->name . "\", var->" . $var_m->name . " );\n";
		}
	}
	$code =+ "} \n\n";
}

$globals =+ "var_hash all_vars = [\n";


$hashed = array();

foreach( $vars_local as $var ) {
	//$hashed_name = hash("crc32", $var->name );
	$hashed[ "".$var->name ] = "".$var->type;
}

$print_prev_line = false;
$prev_line = "";

foreach( $hashed as $hashed_name=>$type_of_hashed_name ) {
	if( $print_prev_line ) {
		$globals =+ $prev_line;
	}
	
	//$prev_line = "    { \"" . $hashed_name . "\", DISPLAY_" . get_var_type_upper($type_of_hashed_name) . " },\n";
	$prev_line = "    { \"" . $hashed_name . "\", \"" . $type_of_hashed_name . "\" },\n";

	$print_prev_line = true;
}

$globals =+ substr_replace( $prev_line, "", -2, 1 );

$globals =+ "];\n";

$var_size = count( $hashed );

echo $defines;
echo $prototypes;
echo $globals;
echo $code;

print<<<END

#define NUM_VARS_IN_CREFLECT $var_size

void find_and_display( char * name, void * var ) {
	//uint32_t hashed_name = crc32( name );

	for( int i = 0; i < NUM_VARS_IN_CREFLECT; i++ ) {
		var_hash *vh = &all_vars[i];

END;

	foreach( $hashed as $name=>$value ) {
		$value_upper = get_var_type_upper( $value );
echo<<<ENDLOOP

		if( strcmp( vh->type, "$value" ) == 0 ) {
			$value * var_resolved = ($value *)var;
			DISPLAY_{$value_upper}( name, var_resolved )
		}

ENDLOOP;
	}
print<<<END2
	} 
}


END2;


function get_var_type_upper( $type ) {
	$ret = $type;

	$ret = strtolower( $type );

	if( str_contains( $type, "char *" ) ) {
		$ret = "string";
	}

	if( str_contains( $type, "char*" ) ) {
		$ret = "string";
	}

	if( str_contains( $type, "unicode_info *" ) ) {
		$ret = "int";
	}

	if( str_contains( $type, "unicode_info*" ) ) {
		$ret = "int";
	}

	return strtoupper( $ret );
}