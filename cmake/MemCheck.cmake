
find_program(MEMCHECK_COMMAND valgrind)

function(add_memcheck_test _cond _targetname _testrunner)
  if (NOT ${_cond} OR NOT MEMCHECK_COMMAND)
    add_test(${_targetname} ${_testrunner} ${ARGV3})
  else ()
    add_test(${_targetname} ${MEMCHECK_COMMAND} --leak-check=full --error-exitcode=5 ${ARGV3} --quiet ${_testrunner})
  endif()

endfunction()
