:: individual components
@setlocal
@set INC=%PROGRAMS%\local\include
@if not exist %INC%\stdshit mkdir %INC%\stdshit
@copy /Y src\min_max.h %INC%\stdshit
