# Script for the automatic creation of test configurations
# Checks the modules in the modules directory specified in the constants
# section below (typically "modules"), and creates several
# test configurations:
#   - one with all modules enabled
#   - one with no modules enabled
#   - one for each module with only the module (plus its dependencies) enabled
# In a second step, a script is created to run all those test configurations.

import os, re, sys

if (len(sys.argv) != 4 ):
	print("Invalid number of arguments")
	print("Expected Syntax:")
	print("  $ CreateTestConfiguration.py <SrcDir> <BranchName> <ConfigOutFolder>")
	sys.exit(1)

SrcDir = sys.argv[1]
GitBranchName = sys.argv[2]
ConfigOutFolder = sys.argv[3]

# Constants:
ModuleDir = SrcDir+'/modules'
RunnerScriptLinux = SrcDir + '/Test_files/TestRunner.sh'
RunnerScriptWindows = SrcDir + '/Test_files/TestRunner.bat'
KeyValidation = 'KeyValidation'

AllModulesOnScript = 'all_flags.cmake'
AllModulesOffScript = 'no_flags.cmake'

print "CreateTestConfiguration script v0.1"

moduleNames = os.listdir(ModuleDir)

# write cmake file for enabling all modules:
with open(ConfigOutFolder+'/'+AllModulesOnScript, 'w') as file:
	file.write('SET (SITE "${FIX_SITE}_'+GitBranchName+'_AllModules" CACHE STRING "" FORCE)\n')
	for dirname in moduleNames:
		file.write('SET (Module_'+dirname+' "ON" CACHE BOOL "" FORCE)\n')

# write cmake file for disabling all modules:
with open(ConfigOutFolder+'/'+AllModulesOffScript, 'w') as file:
	file.write('SET (SITE "${FIX_SITE}_'+GitBranchName+'_NoModules" CACHE STRING "" FORCE)\n\n')
	for dirname in moduleNames:
		file.write('SET (Module_'+dirname+' "OFF" CACHE BOOL "" FORCE)\n')

# determine dependencies for each module
dependencies = dict()
for module in moduleNames:
	#print(module)
	DepFileName = ModuleDir+'/'+module+'/Dependencies.cmake'
	if os.path.isfile(DepFileName):
		with open(DepFileName) as depfile:
			data = depfile.read()
		m = re.search(r"DEPENDENCIES_MODULES\s+([^)]+)", data, re.MULTILINE)
		if m:
			dependencies[module] = m.group(1).split()

# recursively resolve dependencies:
recursiveDeps = dict()
for key in dependencies:
	depstack = list(dependencies[key])
	recursiveDeps[key] = list()
	while (depstack):
		subdep = depstack.pop()
		if (subdep == key):
			print key+": Circular dependency"
		if (subdep != key and not subdep in recursiveDeps[key]):
			recursiveDeps[key].append(subdep)
			if (subdep in dependencies):
				depstack.extend(dependencies[subdep])
print

#print "Resolved dependencies:"
#for key in recursiveDeps:
#	print key+": "
#	print recursiveDeps[key]

# write one build config per module:
for module in moduleNames:
	cmakeFileName = ConfigOutFolder+'/Module_'+module+'.cmake'
	with open(cmakeFileName, 'w') as file:
		file.write('SET (SITE "${FIX_SITE}_'+GitBranchName+'_'+module+'" CACHE STRING "" FORCE)\n\n')
		file.write('SET (Module_'+module+' "ON" CACHE BOOL "" FORCE)\n')
		if module in recursiveDeps:
			file.write('\n')
			for submodule in recursiveDeps[module]:
				file.write('SET (Module_'+submodule+' "ON" CACHE BOOL "" FORCE)\n')

print "Done."