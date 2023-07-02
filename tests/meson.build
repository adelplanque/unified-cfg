py3_exe = import('python').find_installation()

test_env = ['CFG=' + cfg.full_path()]

testcases = [
  'test_doc.yaml',
  'test_env.yaml',
  'test_get.yaml',
  'test_get2.yaml',
  'test_get_errors.yaml',
  'test_hardware.yaml',
  'test_list.yaml',
  'test_list_empty.yaml',
  'test_list_tabulate.yaml',
  'test_multilines.yaml',
  'test_recursive.yaml',
  'test_render.yaml',
  'test_shell.yaml',
  'test_strings.yaml',
  'test_tmpl_errors.yaml',
]

foreach testcase : testcases
  test(testcase, py3_exe, args: files('run-test.py', testcase), env: test_env)
endforeach
