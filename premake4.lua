-- A solution contains projects, and defines the available configurations
cfg = assert( require 'premake.config' )
actions = assert( require 'premake.actions' )

----------------------------------------------------------------------------------------------------------------
actions.make_solution "selfdestructing"
includedirs { 
	"./selfdestructing",
	"./Catch/single_include"
}
----------------------------------------------------------------------------------------------------------------
actions.make_console_app(
	"selfdestructing-test",
	{ "./test/test.cpp" }
)
actions.make_cpp11()
actions.run_target_after_build()
