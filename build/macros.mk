# ucoolib - Microcontroller object oriented library.
#
# Usefull macros.

# Return $(A) only if defined, else return B.
# $(call defval,A,B)
defval = $(if $(filter undefined,$(origin $1)),$2,$($1))

# Filter out source file for other targets, return basenames.
# $(call filter_sources,TARGET,SOURCES)
filter_sources = $(call filter_sources_sub,$1,$(basename $2))
filter_sources_sub = $(foreach t,$1 $($1_SUBTARGETS),$(filter %.$t,$2)) \
		     $(foreach f,$2,$(if $(findstring .,$f),,$f))

# Test if one of the source is C++, in this case, return cc, else return c.
# $(call iscxx,SOURCES)
iscxx = $(if $(filter %.cc,$1),cc,c)

# Output every parent directories until root is reached.
# $(call parents,DIRS,ROOT)
parents = $(foreach d,$1,$(call parents_sub,$d,$2))
parents_sub = $(if $(filter-out $2,$1),\
	      $(filter-out %/,$1) \
	      $(call parents_sub,$(patsubst %/,%,$(dir $1)),$2))

# Reverse a list.
# $(call reverse,A B C...)
reverse = $(if $1,$(call reverse,$(wordlist 2,$(words $1),$1))) $(firstword $1)

# Run mkdir if directory does not exists.
# $(call mkdir_if_needed,FILE)
mkdir_if_needed = $(if $(wildcard $(dir $1)),,$Qmkdir -p $(dir $1))

# Try to rmdir empty directories.
# $(call rmdir_as_needed,DIRS,ROOT)
rmdir_as_needed = $Qrmdir $(call reverse,$(sort $(call parents,$1,$2))) \
		  $2 2>/dev/null || true
