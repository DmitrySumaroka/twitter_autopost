# Twitter Autopost Module

This allows a node that is being created or edited to be posted to twitter and the link to that node as a tinyurl.

If I add `oauth` module to the settings it goes into a infinete loop of trying to download it

- `drush dl oauth`
- `drush en oauth_common`
- `drush en twitter_autopost`
- Go to `/admin/config/services/twitter/settings` register a new app with twitter.
- Add consumer key and secret
- Go to `/admin/config/services/twitter` and sign in with your twitter
- Go to `/admin/config/services/twitter-autopost` and add your twitter screen name
- Click `Node Settings` to add the nodes you want to have access to autopost


## TODO

- Lets decide if need that `back to settings` link.
- The css file we have:
  - Style it to taste?
- Maybe add the picture of the default like the twitter module has
- Make sure everything that should be, is translatable,
