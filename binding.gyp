{
    "targets": [
        {
            "target_name": "nis",
            "sources": [ "binding.cc" ],
            "include_dirs" : [
                "<!(node -e \"require('nan')\")"
            ]
        }
    ]
}
