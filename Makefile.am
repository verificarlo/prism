# Include macros from the m4 directory
ACLOCAL_AMFLAGS = -I m4

all: highway prism
	
highway:
	$(BAZEL) build @hwy

prism:
	$(BAZEL) build src:prism-all

bazel-bin/prism-dynamic.ll:
	$(BAZEL) build :prism_ir

bazel-bin/prism-static.ll:
	$(BAZEL) build :prism_ir

include_HEADERS= \
	bazel-bin/prism-dynamic.ll \
	bazel-bin/prism-static.ll \
	src/prism_api.h \
	src/prism.h

EXTRA_DIST = \
	bazel-bin/external/+_repo_rules+hwy/libhwy.so \
	bazel-bin/src/libprism-dynamic.so \
	bazel-bin/src/libprism-static.so \
	bazel-bin/src/libprism-dynamic-dbg.so \
	bazel-bin/src/libprism-static-dbg.so

install-exec-hook:
	@echo "Installing prism library"
	install -m 0755 bazel-bin/external/+_repo_rules+hwy/libhwy.so $(DESTDIR)$(libdir)
	install -m 0755 bazel-bin/src/libprism-dynamic-dbg.so $(DESTDIR)$(libdir)
	install -m 0755 bazel-bin/src/libprism-dynamic.so $(DESTDIR)$(libdir)
	install -m 0755 bazel-bin/src/libprism-static-dbg.so $(DESTDIR)$(libdir)
	install -m 0755 bazel-bin/src/libprism-static.so $(DESTDIR)$(libdir)

clean:
	$(BAZEL) clean