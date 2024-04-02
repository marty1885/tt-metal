LOOPBACK_EXAMPLE_SRC = $(TT_METAL_HOME)/tt_metal/programming_examples/contrib/vecadd/vecadd.cpp

LOOPBACK_EXAMPLES_DEPS = $(PROGRAMMING_EXAMPLES_CONTRIB_OBJDIR)/vecadd.d

-include $(LOOPBACK_EXAMPLES_DEPS)

.PRECIOUS: $(PROGRAMMING_EXAMPLES_CONTRIB_TESTDIR)/vecadd
$(PROGRAMMING_EXAMPLES_CONTRIB_TESTDIR)/vecadd: $(PROGRAMMING_EXAMPLES_CONTRIB_OBJDIR)/vecadd.o $(TT_METAL_LIB)
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) $(CXXFLAGS) $(PROGRAMMING_EXAMPLES_INCLUDES) -o $@ $^ $(LDFLAGS) $(PROGRAMMING_EXAMPLES_LDFLAGS)

.PRECIOUS: $(PROGRAMMING_EXAMPLES_CONTRIB_OBJDIR)/vecadd.o
$(PROGRAMMING_EXAMPLES_CONTRIB_OBJDIR)/vecadd.o: $(LOOPBACK_EXAMPLE_SRC)
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) $(CXXFLAGS) $(PROGRAMMING_EXAMPLES_INCLUDES) -c -o $@ $<
