include 'premake'

make_solution 'selfdestructing'

includedirs { 
	'./selfdestructing',
	'./Catch/single_include'
}

make_console_app( 'selfdestructing-test', { './test/test.cpp' } )

use_standard('c++11')

run_target_after_build()
