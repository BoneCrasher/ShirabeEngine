set THIS=%cd%
cd /D C:\Program Files (x86)\Graphviz2.38\bin
dot -v -Tpng -o C:\Workspaces\ShirabeDevelopment\Workspace\_deploy\Win32\Debug\bin\Test.png C:\Workspaces\ShirabeDevelopment\Workspace\_deploy\Win32\Debug\bin\RenderGraphTest.gv
cd /D %THIS%
