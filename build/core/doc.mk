
ALL_DOC_FILES = $(sort $(foreach m,$(sort $(ALL_BUILD_MODULES)),$(ALL_DOC_FILES.$(m))))
.PHONY: doc
doc:
	$(shell echo $(ALL_DOC_FILES) >> .bbbb.tmp)
	@echo "*********************************************************"
	@echo "******Please wait while the document is created..********"
	@doxygen -s -g
	@python $(BUILD_SYSTEM)/tools/creat_doc_list.py
	@python $(BUILD_SYSTEM)/tools/modify_doxyfile.py
	@doxygen
	@echo "*********Successfull !***************"
	@rm .bbbb.tmp .a.tmp .b.tmp .c.tmp Doxyfile
	@echo "********************************************"
