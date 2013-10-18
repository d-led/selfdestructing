assert ( require 'premake.quickstart' )

make_solution 'selfdestructing'

includedirs { 
	'./selfdestructing',
	'./Catch/single_include'
}

make_console_app( 'selfdestructing-test', { './test/test.cpp' } )
make_cpp11()
run_target_after_build()
