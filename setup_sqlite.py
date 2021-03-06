


import setuptools
from setuptools import setup, Extension
import glob, time, platform, os, sysconfig, sys, shutil, io

VERSION = '3.1.47'

#usedir = os.path.dirname(__file__)
#if sys.path[0] != usedir:
#	sys.path.insert(0, usedir)

def file_at(*arg):
	#return os.path.join(usedir, *arg)
	return os.path.join(*arg)


import numpy

if platform.system() == 'Darwin':
	os.environ['LDFLAGS'] = '-framework Accelerate'
	os.environ['CLANG_CXX_LIBRARY'] = 'libc++'
	os.environ['CLANG_CXX_LANGUAGE_STANDARD'] = 'gnu++0x'



compiletime = time.strftime("%A %B %d %Y - %I:%M:%S %p")


from setup_common import lib_folder, shlib_folder, temp_folder


sqlite3_exports=[
'sqlite3_aggregate_context',
'sqlite3_aggregate_count',
'sqlite3_auto_extension',
'sqlite3_backup_finish',
'sqlite3_backup_init',
'sqlite3_backup_pagecount',
'sqlite3_backup_remaining',
'sqlite3_backup_step',
'sqlite3_bind_blob',
'sqlite3_bind_double',
'sqlite3_bind_int',
'sqlite3_bind_int64',
'sqlite3_bind_null',
'sqlite3_bind_parameter_count',
'sqlite3_bind_parameter_index',
'sqlite3_bind_parameter_name',
'sqlite3_bind_text',
'sqlite3_bind_text16',
'sqlite3_bind_value',
'sqlite3_bind_zeroblob',
'sqlite3_blob_bytes',
'sqlite3_blob_close',
'sqlite3_blob_open',
'sqlite3_blob_read',
'sqlite3_blob_reopen',
'sqlite3_blob_write',
'sqlite3_busy_handler',
'sqlite3_busy_timeout',
'sqlite3_cancel_auto_extension',
'sqlite3_changes',
'sqlite3_clear_bindings',
'sqlite3_close',
'sqlite3_close_v2',
'sqlite3_collation_needed',
'sqlite3_collation_needed16',
'sqlite3_column_blob',
'sqlite3_column_bytes',
'sqlite3_column_bytes16',
'sqlite3_column_count',
#'sqlite3_column_database_name',
#'sqlite3_column_database_name16',
'sqlite3_column_decltype',
'sqlite3_column_decltype16',
'sqlite3_column_double',
'sqlite3_column_int',
'sqlite3_column_int64',
'sqlite3_column_name',
'sqlite3_column_name16',
#'sqlite3_column_origin_name',
#'sqlite3_column_origin_name16',
#'sqlite3_column_table_name',
#'sqlite3_column_table_name16',
'sqlite3_column_text',
'sqlite3_column_text16',
'sqlite3_column_type',
'sqlite3_column_value',
'sqlite3_commit_hook',
'sqlite3_compileoption_get',
'sqlite3_compileoption_used',
'sqlite3_complete',
'sqlite3_complete16',
'sqlite3_config',
'sqlite3_context_db_handle',
'sqlite3_create_collation',
'sqlite3_create_collation16',
'sqlite3_create_collation_v2',
'sqlite3_create_function',
'sqlite3_create_function16',
'sqlite3_create_function_v2',
'sqlite3_create_module',
'sqlite3_create_module_v2',
'sqlite3_data_count',
'sqlite3_db_config',
'sqlite3_db_filename',
'sqlite3_db_handle',
'sqlite3_db_mutex',
'sqlite3_db_readonly',
'sqlite3_db_release_memory',
'sqlite3_db_status',
'sqlite3_declare_vtab',
'sqlite3_enable_load_extension',
'sqlite3_enable_shared_cache',
'sqlite3_errcode',
'sqlite3_errmsg',
'sqlite3_errmsg16',
'sqlite3_errstr',
'sqlite3_exec',
'sqlite3_expired',
'sqlite3_extended_errcode',
'sqlite3_extended_result_codes',
'sqlite3_file_control',
'sqlite3_finalize',
'sqlite3_free',
'sqlite3_free_table',
'sqlite3_get_autocommit',
'sqlite3_get_auxdata',
'sqlite3_get_table',
'sqlite3_global_recover',
'sqlite3_initialize',
'sqlite3_interrupt',
'sqlite3_last_insert_rowid',
'sqlite3_libversion',
'sqlite3_libversion_number',
'sqlite3_limit',
'sqlite3_load_extension',
'sqlite3_log',
'sqlite3_malloc',
'sqlite3_memory_alarm',
'sqlite3_memory_highwater',
'sqlite3_memory_used',
'sqlite3_mprintf',
'sqlite3_mutex_alloc',
'sqlite3_mutex_enter',
'sqlite3_mutex_free',
'sqlite3_mutex_leave',
'sqlite3_mutex_try',
'sqlite3_next_stmt',
'sqlite3_open',
'sqlite3_open16',
'sqlite3_open_v2',
'sqlite3_os_end',
'sqlite3_os_init',
'sqlite3_overload_function',
'sqlite3_prepare',
'sqlite3_prepare16',
'sqlite3_prepare16_v2',
'sqlite3_prepare_v2',
'sqlite3_profile',
'sqlite3_progress_handler',
'sqlite3_randomness',
'sqlite3_realloc',
'sqlite3_release_memory',
'sqlite3_reset',
'sqlite3_reset_auto_extension',
'sqlite3_result_blob',
'sqlite3_result_double',
'sqlite3_result_error',
'sqlite3_result_error16',
'sqlite3_result_error_code',
'sqlite3_result_error_nomem',
'sqlite3_result_error_toobig',
'sqlite3_result_int',
'sqlite3_result_int64',
'sqlite3_result_null',
'sqlite3_result_text',
'sqlite3_result_text16',
'sqlite3_result_text16be',
'sqlite3_result_text16le',
'sqlite3_result_value',
'sqlite3_result_zeroblob',
'sqlite3_rollback_hook',
#'sqlite3_rtree_geometry_callback',
#'sqlite3_rtree_query_callback',
'sqlite3_set_authorizer',
'sqlite3_set_auxdata',
'sqlite3_shutdown',
'sqlite3_sleep',
'sqlite3_snprintf',
'sqlite3_soft_heap_limit',
'sqlite3_soft_heap_limit64',
'sqlite3_sourceid',
'sqlite3_sql',
'sqlite3_status',
'sqlite3_step',
'sqlite3_stmt_busy',
'sqlite3_stmt_readonly',
'sqlite3_stmt_status',
'sqlite3_strglob',
'sqlite3_stricmp',
'sqlite3_strnicmp',
#'sqlite3_table_column_metadata',
'sqlite3_test_control',
'sqlite3_thread_cleanup',
'sqlite3_threadsafe',
'sqlite3_total_changes',
'sqlite3_trace',
'sqlite3_transfer_bindings',
'sqlite3_update_hook',
'sqlite3_uri_boolean',
'sqlite3_uri_int64',
'sqlite3_uri_parameter',
'sqlite3_user_data',
'sqlite3_value_blob',
'sqlite3_value_bytes',
'sqlite3_value_bytes16',
'sqlite3_value_double',
'sqlite3_value_int',
'sqlite3_value_int64',
'sqlite3_value_numeric_type',
'sqlite3_value_text',
'sqlite3_value_text16',
'sqlite3_value_text16be',
'sqlite3_value_text16le',
'sqlite3_value_type',
'sqlite3_vfs_find',
'sqlite3_vfs_register',
'sqlite3_vfs_unregister',
'sqlite3_vmprintf',
'sqlite3_vsnprintf',
'sqlite3_vtab_config',
'sqlite3_vtab_on_conflict',
'sqlite3_wal_autocheckpoint',
'sqlite3_wal_checkpoint',
'sqlite3_wal_checkpoint_v2',
'sqlite3_wal_hook',
#'sqlite3_win32_is_nt',
'sqlite3_win32_mbcs_to_utf8',
'sqlite3_win32_set_directory',
'sqlite3_win32_sleep',
'sqlite3_win32_utf8_to_mbcs',
'sqlite3_win32_write_debug',

'sqlite3_haversine_autoinit',
'sqlite3_bonus_autoinit',

]



if platform.system() == 'Darwin':
	openblas = None
	gfortran = None
	mingw64_libs = []
	local_swig_opts = []
	local_libraries = []
	local_library_dirs = []
	local_includedirs = []
	local_macros = [('I_AM_MAC','1'), ('SQLITE_ENABLE_RTREE','1'), ]
	local_extra_compile_args = ['-std=gnu++11', '-w', '-arch', 'i386', '-arch', 'x86_64']# +['-framework', 'Accelerate']
	local_apsw_compile_args = ['-w']
	local_extra_link_args =   ['-framework', 'Accelerate']
	local_data_files = [('/usr/local/bin', ['bin/larch']), ]
	local_sqlite_extra_postargs = []
	dylib_name_style = "lib{}.so"
	DEBUG = False
	buildbase = None
elif platform.system() == 'Windows':
	#old openblas = 'OpenBLAS-v0.2.9.rc2-x86_64-Win', 'lib', 'libopenblas.dll'
	openblas = 'OpenBLAS-v0.2.15-Win64-int32', 'lib', 'libopenblas.dll'
	#gfortran = 'OpenBLAS-v0.2.9.rc2-x86_64-Win', 'lib', 'libgfortran-3.dll'
	gfortran = 'OpenBLAS-v0.2.15-Win64-int32', 'lib', 'libgfortran-3.dll'
	mingw64_path = 'OpenBLAS-v0.2.15-Win64-int32', 'lib',
	mingw64_dlls = ['libgfortran-3', 'libgcc_s_seh-1', 'libquadmath-0']
	mingw64_libs = [i+'.dll' for i in mingw64_dlls]
	local_swig_opts = []
	local_libraries = ['PYTHON3{}'.format(sys.version_info.minor),'libopenblas',]+mingw64_dlls+['PYTHON3{}'.format(sys.version_info.minor),]
	local_library_dirs = [
		'Z:/CommonRepo/{0}/{1}'.format(*openblas),
	#	'C:\\local\\boost_1_56_0\\lib64-msvc-10.0',
		]
	local_includedirs = [
		'Z:/CommonRepo/{0}/include'.format(*openblas),
	#	'C:/local/boost_1_56_0',
		 ]
	local_macros = [('I_AM_WIN','1'),  ('SQLITE_ENABLE_RTREE','1'), ]
	local_extra_compile_args = ['/EHsc', '/W0', ]
	#  for debugging...
	#	  extra_compile_args=['/Zi' or maybe '/Z7' ?],
	#     extra_link_args=[])
	local_apsw_compile_args = ['/EHsc']
	local_extra_link_args =    ['/DEBUG']
	local_data_files = []
	local_sqlite_extra_postargs = [] #['/IMPLIB:' + os.path.join(shlib_folder(basepath), 'larchsqlite.lib'), '/DLL',]
	dylib_name_style = "{}.dll"
	DEBUG = False
	buildbase = None # "Z:\LarchBuild"
#	raise Exception("TURN OFF multithreading in OpenBLAS")
else:
	openblas = None
	gfortran = None
	mingw64_libs = []
	local_swig_opts = []
	local_libraries = []
	local_library_dirs = []
	local_includedirs = []
	local_macros = [('I_AM_LINUX','1'),  ('SQLITE_ENABLE_RTREE','1'), ]
	local_extra_compile_args = []
	local_apsw_compile_args = []
	local_extra_link_args =    []
	local_data_files = []
	local_sqlite_extra_postargs = []
	dylib_name_style = "{}.so"
	DEBUG = False
	buildbase = None





shared_libs = [
('larchsqlite', [file_at('sqlite','sqlite3.c'),file_at('sqlite','haversine.c'),file_at('sqlite','bonus.c')] ,sqlite3_exports,  local_sqlite_extra_postargs, []),
]




def build_sqlite(basepath=None):
	if basepath is None:
		basepath = buildbase
	from distutils.ccompiler import new_compiler

	# Create compiler with default options
	c = new_compiler()
	c.add_include_dir(file_at('sqlite'))

	for name, source, exports, extra_postargs, extra_preargs in shared_libs:
		
		if not isinstance(source,list):
			source = [source,]
		
		need_to_update = False
		for eachsource in source:
			try:
				print("checking filemod time on",eachsource,' vs ',os.path.join(shlib_folder(basepath), dylib_name_style.format(name)))
				need_to_update = need_to_update or (os.path.getmtime(eachsource) > os.path.getmtime(os.path.join(shlib_folder(basepath), dylib_name_style.format(name))))
			except FileNotFoundError:
				need_to_update = True

		# change dynamic library install name
		if platform.system() == 'Darwin':
			extra_postargs += ['-install_name', '@loader_path/{}'.format(c.library_filename(name,'shared'))]
			extra_preargs  += ['-arch', 'i386', '-arch', 'x86_64']

		if platform.system() == 'Windows':
			extra_postargs = ['/IMPLIB:' + os.path.join(shlib_folder(basepath), 'larchsqlite.lib'), '/DLL',]


		if need_to_update:
			print("yes need to update")
			print("temp_folder(basepath)=",temp_folder(basepath))
			print("shlib_folder(basepath)=",shlib_folder(basepath))
			# Compile into .o files
			objects = c.compile(source, extra_preargs=extra_preargs, debug=DEBUG, macros=local_macros, output_dir=temp_folder(basepath))
			# Create shared library
			c.link_shared_lib(objects, name, output_dir=shlib_folder(basepath), export_symbols=exports, extra_preargs=extra_preargs, extra_postargs=extra_postargs, debug=DEBUG)
		else:
			print("no need to update")
			print("temp_folder(basepath)=",temp_folder(basepath))
			print("shlib_folder(basepath)=",shlib_folder(basepath))


if __name__=="__main__":
	print("building larchsqlite...")
	try:
		import sys
		if len(sys.argv)>1:
			shlib_folder = lambda x: sys.argv[1]
		build_sqlite()
	except:
		exit(-1)
	exit(0)
