# -*- coding: UTF-8 -*-
# custom_json.py
# MyEncoder, MyDecoder for custom classes.

import json
import importlib
from basic import *

class MyEncoder(json.JSONEncoder):
    def default(self, obj):
        d = { '__class__':obj.__class__.__name__,
              '__module__':obj.__module__ }
        d.update(obj.__dict__)
        return d

class MyDecoder(json.JSONDecoder):
    def __init__(self):
        super(MyDecoder, self).__init__(object_hook = self.dict_to_object)

    def dict_to_object(self, d):
        if '__class__' in d:
            class_name = d.pop('__class__')
            module_name = d.pop('__module__')
            module = importlib.import_module(module_name)
            class_ = getattr(module, class_name)
            args = dict((key.encode('ascii'), value) for key, value in d.items())
            inst = class_(**args)
        else:
            inst = d
        return inst
