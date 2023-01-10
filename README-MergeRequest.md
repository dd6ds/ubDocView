# Prerequisites to approving a Merge Request (MR)

Over time, it has been found that insufficient testing by reviewers sometimes
leads to doc viewer app trunk not buildable in Qtcreator due to manifest
errors, or translation pot file not updated. As such, please follow the
checklist below before approving a MR.

# Checklist

*   Does the MR add/remove user visible strings? If Yes, the maintainer who
    merges the MR should update the pot file afterwards and push the change
    directly to the target branch.

*   Does the MR change the UI? If Yes, has it been discussed with some of the
    DocViewer developers?

*   Did you perform an exploratory manual test run of your code change and any
    related functionality?

*   If the MR fixes a bug or implements a feature, are there accompanying unit
    and autopilot tests?

*   Is the app buildable and runnable using Qtcreator?

*   Was the copyright years updated if necessary?

The above checklist is more of a guideline to help doc viewer app trunk stay buildable,
stable and up to date.
